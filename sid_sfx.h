/*
 * sid_sfx.h  –  Non-blocking SID sound effects for gridgame
 *
 * Usage:
 *   1. Call sid_sfx_init() once at startup (after clrscr/setup).
 *   2. Call sid_waterdrop() whenever you want the effect to play.
 *      It may be called again before the previous one finishes;
 *      the sound simply restarts from the top.
 *   3. Call sid_tick() once per frame, every frame, forever.
 *      The simplest place is inside your vsyncw() replacement — see
 *      notes in sid_sfx.c about where to put this in gridgame.
 *
 * This file is compiled only for C-64 and C-128 targets.
 * On CX-16 the functions are defined as empty stubs so the rest of
 * the code compiles unchanged.
 */

#ifndef SID_SFX_H
#define SID_SFX_H

/* ------------------------------------------------------------------ */
/*  Only real SID hardware on C-64 / C-128                            */
/* ------------------------------------------------------------------ */
#if defined(__C64__) || defined(__C128__)

/* Call once at startup to silence the SID and set master volume. */
void sid_sfx_init(void);

/*
 * Trigger a water-drop "bloop" on SID Voice 1.
 * Safe to call while one is already playing — it just restarts.
 * Returns immediately; the actual sound plays over subsequent frames
 * driven by sid_tick().
 */
void sid_waterdrop(void);

/*
 * Advance the sound-effect state machine by one frame.
 * Call this exactly once per frame — i.e. once per vsyncw() call.
 * Takes < 20 CPU cycles when no sound is active.
 */
void sid_tick(void);

#else  /* CX-16 or other targets: stubs so callers need no #ifdefs */

#define sid_sfx_init()   ((void)0)
#define sid_waterdrop()  ((void)0)
#define sid_tick()       ((void)0)

#endif /* __C64__ || __C128__ */

#endif /* SID_SFX_H */
