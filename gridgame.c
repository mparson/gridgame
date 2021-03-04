/*
 The code for this game is Copyright ©2021, Michael Parson
 <mparson@bl.org>.

 This game is free software; you can redistribute it and/or modify it
 however you see fit. All I ask is that you give credit back to myself
 and anyone else that has contributed to the game.

 This game is distributed in the hope that it will be at least mildly
 entertaining, but WITHOUT ANY WARRANTY; without even the implied
 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 Michael Parson can not be held responsible for any lost data, wasted
 time, lost friendships, ills, wars, fires, or other Bad Things™ that
 may or may not occur before, during, or after acquisition of this
 software.

 Play at your own risk, no lifeguard on duty.
 */

#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __CX16__
#include <mouse.h>
bool global_b;
#endif

#if defined (__C64__) || defined (__C128__)
#include <joystick.h>
#include "c64/joy.h"
#endif

#include "boardedit.h"
#include "global.h"
#include "button.h"
#include "draw.h"
#include "board.h"
#include "queue.h"

int main () {
	#ifdef __CX16__
	struct mouse_info info;
	unsigned char mbl;
	#endif
	unsigned char c,mx,my;

	Queue *Q = createQueue (63);

	clrscr ();
	// kick us back into upper-case/PETSCII mode
	__asm__ ("lda #$8E");
	__asm__ ("jsr $FFD2");

	#if defined (__C64__) || defined (__C128__)
	textcolor (COLOR_WHITE);
	bordercolor (COLOR_BLUE);
	bgcolor (COLOR_BLUE);
	joy_install (joy_static_stddrv);
	#endif

	// only use mouse on the X-16 (save memory for the C-64 version)
	#ifdef __CX16__
	videomode (0);
	mouse_load_driver (&mouse_def_callbacks, mouse_stddrv);
	mouse_install (&mouse_def_callbacks,mouse_static_stddrv);
	mouse_show ();
	setupchars ();
	#endif

	_randomize ();

	loadhs ();
	global_nhs = true;
	updatescore ();
	global_nhs = false;
	global_newrandboard = true;
	updateboard ();

	while (1) {
		#ifdef __CX16__
		mbl = 0;
		mouse_info (&info);
		mbl = (info.buttons & MOUSE_BTN_LEFT);
		if (mbl) {
			global_b = true;
			mx = info.pos.x >> 3;
			my = info.pos.y >> 3;
		}
		#endif

		#if defined (__C64__) || defined (__C128__)
		joyinfo ();

		mx = (unsigned char) jx;
		my = (unsigned char) jy;
		#endif

		vsyncw (5);

		// check for clicks on gameboard
		if (global_b) {
			if ((mx >= LX && mx <= HX) && (my >= LY && my <= HY)) {
				cclearxy (2,16,5);  // clear the 'ready' text
				global_score = 1;
				updatescore ();
				gotoxy (mx,my);
				c = cpeekc ();
				textcolor (COLOR_RED);
				cputcxy (mx,my,c);
				textcolor (COLOR_WHITE);
				Enqueue (Q,mx,my);
				processQ (Q);
			} else {
				mbutton (mx,my);
			}
		}
		if (global_nhs) {
			#ifndef __CX16__
			cputsxy (2,16,"new hs");
			cputsxy (2,17,"saving");
			#endif
			savehs ();
			#ifndef __CX16__
			cclearxy (2,16,6); // clear the 'new hs' text.
			cclearxy (2,17,6); // clear the 'saving' text.
			#endif
		}

		global_b = false;
		cputsxy (2,16,"ready");
	}
	return 0;
}
