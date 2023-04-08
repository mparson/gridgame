#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>

#define LX  12	// low 'x' val (left side of playfield)
#define HX  27	// high 'x' val (right side of playfield)
#define LY  1	// low 'y' val (top of playfield)
#define HY  16	// high 'y' val (bottom of playfield)

// For some reason, cpeekcolor () returns '98' for COLOR_RED text instead of '2'.
#ifdef  __CX16__
#undef COLOR_RED
#define COLOR_RED   98
#endif

extern unsigned int global_hscore;
extern unsigned int global_score;
//extern bool global_overwrite;
extern bool global_nhs;					// new high score is set
extern bool global_editmode;			// we're in edit mode
extern bool global_newrandboard;		// we want a new random board
extern bool global_b;					// mouse/joy button pressed

extern void vsyncw (unsigned char c);	// Number of frames to wait before doing stuff

#endif
