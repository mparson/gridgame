/*
 * sid_sfx.c  –  Non-blocking SID sound effects for gridgame
 *
 * HOW IT WORKS
 * ------------
 * Instead of busy-waiting, all sound work is broken into per-frame
 * "ticks."  sid_waterdrop() just loads a few state variables and
 * returns immediately.  sid_tick() checks the C64 system jiffy clock
 * to see if a new frame has elapsed, and if so advances the pitch
 * sweep by one step.
 *
 * WHY NOT HOOK vsyncw()?
 * ----------------------
 * processQ() computes:
 *
 *     vs = (15 * (Q->size - 25)) / -100;
 *
 * When Q->size is less than ~25 (which is most of the time during
 * normal play), this evaluates to 0.  vsyncw(0) on cc65/C64 returns
 * immediately without waiting for any frame boundary, so a wrapper
 * around vsyncw() would never call sid_tick() during those iterations.
 * The state machine would stall mid-sweep and subsequent calls to
 * sid_waterdrop() would find it stuck, causing only the first bloop
 * to ever be heard.
 *
 * THE JIFFY CLOCK FIX
 * -------------------
 * The C64 KERNAL IRQ increments a 3-byte counter at $A0/$A1/$A2
 * (most-significant byte at $A0) 50 or 60 times per second (PAL/NTSC).
 * We read only the least-significant byte ($A2) as our frame ticker.
 * sid_tick() compares it against the last value seen; if it has
 * changed, a new frame has elapsed and we advance the sweep by one
 * step.  This is completely independent of vsyncw() and costs only a
 * PEEK and a compare when nothing has changed.
 *
 * CALL PATTERN
 * ------------
 * Call sid_tick() once per processQ() loop iteration (right before
 * Deqeue) AND once per frame in the main loop alongside vsyncw().
 * Calling it more often than once per frame is harmless — the jiffy
 * guard makes extra calls free.
 *
 * THREAD / IRQ SAFETY
 * -------------------
 * The KERNAL IRQ only writes to $A0-$A2; it never touches SID
 * registers, so no SEI/CLI guards are needed here.
 *
 * SID VOICE USED: Voice 1 ($D400-$D406).
 * Voices 2 and 3 are untouched; use them for music freely.
 */

#include "sid_sfx.h"

#if defined(__C64__) || defined(__C128__)

#include <peekpoke.h>   /* POKE / PEEK macros — part of cc65 */

/* ------------------------------------------------------------------
 * SID register addresses – Voice 1
 * ------------------------------------------------------------------ */
#define SID_V1_FREQLO   0xD400u
#define SID_V1_FREQHI   0xD401u
#define SID_V1_CTRL     0xD404u
#define SID_V1_AD       0xD405u   /* Attack (hi) / Decay (lo) nibbles  */
#define SID_V1_SR       0xD406u   /* Sustain (hi) / Release (lo) nibbles */
#define SID_VOL         0xD418u   /* Master volume (low nibble), filter  */

/* KERNAL jiffy clock – LSB at $A2 increments every frame (50/60 Hz) */
#define JIFFY_LSB       0x00A2u

/* Control register bits */
#define CTRL_GATE       0x01
#define CTRL_TRIANGLE   0x10

/* ------------------------------------------------------------------
 * Sound shape parameters
 *
 * The water-drop sweep spans FREQ_START → FREQ_END over SWEEP_FRAMES
 * frames.  At 50 Hz (PAL) that is SWEEP_FRAMES/50 seconds of sweep.
 *
 * SID frequency word ≈ desired_Hz × 16 777 216 / clock_Hz
 *   PAL  clock = 985 248 Hz  →  1 Hz ≈ 17.03 counts
 *   NTSC clock = 1 022 727 Hz →  1 Hz ≈ 16.39 counts
 * The values below split the difference and sound fine on both.
 *
 *   FREQ_START ≈ 0x1800  →  ~1 450 Hz  (the bright initial "plink")
 *   FREQ_END   ≈ 0x0300  →  ~  188 Hz  (the deep final "bloop")
 *
 * SWEEP_FRAMES = 12 gives about 240 ms at 50 Hz — snappy and cartoon-
 * like.  Raise it for a slower, more watery drip.
 *
 * RELEASE_FRAMES is how many frames we wait after clearing the gate
 * before we hard-silence the voice.  Matches the SID release time set
 * in ADSR below (Release nibble 4 ≈ 38 ms ≈ 2 PAL frames; we give it
 * a few extra frames of margin).
 * ------------------------------------------------------------------ */
#define FREQ_START      0x1800u
#define FREQ_END        0x0300u
#define SWEEP_FRAMES    12          /* frames of pitch-fall         */
#define RELEASE_FRAMES  5           /* frames for SID release tail  */

/* Derived: how much to subtract from the frequency word each frame */
#define FREQ_STEP       ((unsigned int)((FREQ_START - FREQ_END) / SWEEP_FRAMES))

/* ------------------------------------------------------------------
 * State machine
 * ------------------------------------------------------------------ */
typedef enum {
    SFX_IDLE = 0,   /* nothing playing                              */
    SFX_SWEEP,      /* pitch is falling; frames_left = ticks left   */
    SFX_RELEASE     /* gate is off; waiting for SID envelope to end */
} SfxState;

static SfxState      sfx_state       = SFX_IDLE;
static unsigned char sfx_frames_left = 0;   /* frames remaining in current phase */
static unsigned int  sfx_freq        = 0;   /* current SID frequency word        */
static unsigned char sfx_last_jiffy  = 0;   /* last jiffy LSB seen               */

/* ------------------------------------------------------------------
 * sid_sfx_init
 * ------------------------------------------------------------------ */
void sid_sfx_init(void)
{
    /* Silence all three voices so we start clean */
    POKE(0xD404u, 0x00);   /* V1 ctrl – gate off, no wave */
    POKE(0xD40Bu, 0x00);   /* V2 ctrl */
    POKE(0xD412u, 0x00);   /* V3 ctrl */

    /* Master volume max, no filter */
    POKE(SID_VOL, 0x0F);

    sfx_state      = SFX_IDLE;
    sfx_last_jiffy = PEEK(JIFFY_LSB);
}

/* ------------------------------------------------------------------
 * sid_waterdrop  –  trigger (or re-trigger) the effect
 *
 * Safe to call at any time, including while already playing.
 * The sweep restarts from the top.
 *
 * IMPORTANT — gate cycling:
 * The SID only fires the attack envelope on a low-to-high (0→1)
 * transition of the gate bit.  If we just write GATE=1 while it is
 * already 1 (i.e. a sound is already playing), nothing happens and
 * the re-trigger is silent.  We must briefly clear the gate first so
 * the next write to set it produces the required 0→1 edge.  The
 * two consecutive POKEs happen within a handful of CPU cycles — far
 * too fast for the human ear to notice a gap.
 *
 * NOTE — we deliberately do NOT reset sfx_last_jiffy here.
 * The jiffy baseline must keep running continuously so that
 * sid_tick() advances on real elapsed time.  If we reset it on every
 * trigger, rapid re-triggers (multiple pieces per frame) all stamp
 * the same jiffy value, the state machine never sees the clock
 * advance, and the sound only ever plays once per click.
 * ------------------------------------------------------------------ */
void sid_waterdrop(void)
{
    /*
     * ADSR for a plucked-drop character:
     *   Attack  = 0  → 2 ms   (instant onset)
     *   Decay   = 3  → 24 ms  (quick fade)
     *   Sustain = 0  → 0      (no sustain – pure pluck)
     *   Release = 4  → 38 ms  (short tail after gate closes)
     */
    POKE(SID_V1_AD, 0x03);
    POKE(SID_V1_SR, 0x04);

    /* Load starting frequency */
    sfx_freq = FREQ_START;
    POKE(SID_V1_FREQLO, (unsigned char)(sfx_freq & 0xFFu));
    POKE(SID_V1_FREQHI, (unsigned char)(sfx_freq >> 8));

    /* Gate cycle: clear gate then set it to guarantee a 0→1 edge
     * and a fresh attack regardless of prior state. */
    POKE(SID_V1_CTRL, CTRL_TRIANGLE);              /* gate OFF */
    POKE(SID_V1_CTRL, CTRL_TRIANGLE | CTRL_GATE);  /* gate ON  */

    sfx_frames_left = SWEEP_FRAMES;
    sfx_state       = SFX_SWEEP;
    /* Do NOT reset sfx_last_jiffy here — see note above. */
}

/* ------------------------------------------------------------------
 * sid_tick  –  call as often as you like (e.g. every processQ loop
 *              iteration).  Real work happens at most once per jiffy
 *              frame; extra calls cost only a PEEK + compare (~10
 *              cycles) and return immediately.
 * ------------------------------------------------------------------ */
void sid_tick(void)
{
    unsigned char jiffy;

    if (sfx_state == SFX_IDLE) {
        return;   /* ~6 cycles — essentially free */
    }

    /* Has a new frame elapsed? */
    jiffy = PEEK(JIFFY_LSB);
    if (jiffy == sfx_last_jiffy) {
        return;   /* same frame — nothing to do yet */
    }
    sfx_last_jiffy = jiffy;

    /* --- One frame's worth of work --- */
    switch (sfx_state) {

        case SFX_SWEEP:
            /* Step frequency down */
            if (sfx_freq > FREQ_END + FREQ_STEP) {
                sfx_freq -= FREQ_STEP;
            } else {
                sfx_freq = FREQ_END;
            }
            POKE(SID_V1_FREQLO, (unsigned char)(sfx_freq & 0xFFu));
            POKE(SID_V1_FREQHI, (unsigned char)(sfx_freq >> 8));

            if (--sfx_frames_left == 0) {
                /* Sweep done — release the gate, let SID fade out */
                POKE(SID_V1_CTRL, CTRL_TRIANGLE);   /* gate bit cleared */
                sfx_frames_left = RELEASE_FRAMES;
                sfx_state       = SFX_RELEASE;
            }
            return;

        case SFX_RELEASE:
            if (--sfx_frames_left == 0) {
                /* Hard silence — prevents stray DC or bleed */
                POKE(SID_V1_CTRL,   0x00u);
                POKE(SID_V1_FREQLO, 0x00u);
                POKE(SID_V1_FREQHI, 0x00u);
                sfx_state = SFX_IDLE;
            }
            return;

        default:
            return;
    }
}

#endif /* __C64__ || __C128__ */


/* ==================================================================
 *  INTEGRATION NOTES FOR gridgame
 * ==================================================================
 *
 * 1. ADD THE FILES
 *    Drop sid_sfx.c and sid_sfx.h into the root of the gridgame
 *    source tree alongside queue.c, board.c, etc.
 *    The Makefile's wildcard rule picks up every *.c automatically:
 *
 *        ALL_OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
 *
 *    So sid_sfx.c will be compiled and linked with no Makefile edits.
 *
 * 2. INITIALISE AT STARTUP  (gridgame.c)
 *    Add the include and one init call near the top of main():
 *
 *        #include "sid_sfx.h"
 *        ...
 *        int main() {
 *            ...
 *            sid_sfx_init();   // ← add after clrscr() / setup
 *            ...
 *        }
 *
 *    NOTE: The vsyncw() wrapper suggested in the previous version of
 *    this library is no longer needed and should be removed if you
 *    added it.
 *
 * 3. TRIGGER AND TICK IN processQ()  (queue.c)
 *
 *    Add to the top of queue.c:
 *
 *        #include "sid_sfx.h"
 *
 *    Inside processQ()'s while loop, add both calls just before
 *    Deqeue():
 *
 *        sid_waterdrop();    // re/trigger the bloop for this piece
 *        sid_tick();         // advance any in-progress sweep;
 *                            // jiffy-gated so free to call every
 *                            // iteration regardless of Q->size
 *        Deqeue(Q);
 *
 *    Why both on every iteration?  processQ() computes:
 *
 *        vs = (15 * (Q->size - 25)) / -100;
 *
 *    When Q->size < 25 (most of normal play), vs == 0 and vsyncw(0)
 *    returns immediately without yielding a frame.  Hooking sid_tick()
 *    to vsyncw therefore misses most calls.  Calling sid_tick()
 *    directly in the loop, guarded by the jiffy clock, is the fix.
 *
 * 4. KEEP TICKING IN THE MAIN LOOP  (gridgame.c)
 *    So the release tail plays out after processQ() returns, add a
 *    sid_tick() call alongside each vsyncw() in the main game loop:
 *
 *        vsyncw(somevalue);
 *        sid_tick();
 *
 * 5. SUMMARY OF ALL CHANGES
 *
 *    File            Change
 *    --------------- -------------------------------------------------
 *    sid_sfx.h       NEW – add to repo
 *    sid_sfx.c       NEW – add to repo (this file)
 *    gridgame.c      #include "sid_sfx.h"
 *                    sid_sfx_init() call in main()
 *                    sid_tick() call alongside each vsyncw() in main loop
 *                    Remove vsyncw_sfx wrapper if previously added
 *    queue.c         #include "sid_sfx.h"
 *                    sid_waterdrop() + sid_tick() before Deqeue() in
 *                    processQ()'s while loop
 *    Makefile        No changes needed.
 *
 * ==================================================================
 */
