/*
 The code for this game is Copyright ©2023, Michael Parson
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

#ifdef MOUSEC
#include <mouse.h>
bool global_b;
#endif

#ifdef JOYC
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
	#ifdef MOUSEC
	struct mouse_info info;
	unsigned char mbl;
	#endif
	unsigned char c,mx,my;

	Queue *Q = createQueue (64);

	clrscr ();
	// kick us back into upper-case/PETSCII mode
	cbm_k_bsout (CH_FONT_UPPER);

	textcolor (COLOR_WHITE);
	bordercolor (COLOR_BLUE);
	bgcolor (COLOR_BLUE);

	#if defined JOYC
	joy_install (joy_static_stddrv);
	#endif

	#ifdef __CX16__
	videomode (0);
	setupchars ();
	#endif

	#ifdef MOUSEC
	mouse_load_driver (&mouse_def_callbacks, mouse_stddrv);
	mouse_install (&mouse_def_callbacks,mouse_static_stddrv);
	mouse_show ();
	#endif

	_randomize ();

	loadhs ();
	global_nhs = true;
	updatescore ();
	global_nhs = false;
	global_newrandboard = true;
	updateboard ();

	while (1) {
		#ifdef MOUSEC
		mbl = 0;
		mouse_info (&info);
		mbl = (info.buttons & MOUSE_BTN_LEFT);
		if (mbl) {
			global_b = true;
			mx = info.pos.x >> 3;
			my = info.pos.y >> 3;
		}
		#endif

		#if defined JOYC
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
				#ifdef MOUSEC
				mouse_hide ();
                #endif
				processQ (Q);
				#ifdef MOUSEC
				mouse_show ();
                #endif
			} else {
				mbutton (mx,my);
			}
		}

		global_b = false;
		cputsxy (2,16,"ready");
	}
	return 0;
}
