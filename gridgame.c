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

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <mouse.h>
#include <string.h>
#include <stdbool.h>

#include "global.h"
#include "button.h"
#include "draw.h"
#include "board.h"
#include "boardedit.h"
#include "queue.h"

int main () {
	struct mouse_info info;
	unsigned char c,mbl,mx,my;

	Queue *Q = createQueue (127);

	#ifdef __CX16__
	videomode (0);
	#endif

	clrscr ();

	// kick us back into upper-case/PETSCII mode
	cbm_k_bsout (CH_FONT_UPPER);

	#ifndef __CX16__
	textcolor (COLOR_WHITE);
	bordercolor (COLOR_BLUE);
	bgcolor (COLOR_BLUE);
	#endif

	_randomize ();

	mouse_load_driver (&mouse_def_callbacks, mouse_stddrv);
	mouse_install (&mouse_def_callbacks,mouse_static_stddrv);
	mouse_show ();

	loadhs ();
	global_newrandboard = true;
	updateboard ();
	
	while (1) {
		mbl = 0;
		mouse_info (&info);
		mbl = (info.buttons & MOUSE_BTN_LEFT);
		for (c = 0; c <= 5; c++) {
			waitvsync ();
		}
		if (mbl) {
			mx = info.pos.x >> 3;
			my = info.pos.y >> 3;

			// check for clicks on gameboard
			if ((mx >= LX && mx <= HX) && (my >= LY && my <= HY)) {
				cclearxy (2,16,5);  // clear the 'ready' text 
				global_score = 0;
				updatescore ();
				gotoxy (mx,my);
				c = cpeekc ();
				textcolor (COLOR_RED);
				cputcxy (c,mx,my);
				textcolor (COLOR_WHITE);
				Enqueue (Q,mx,my);
				processQ (Q);
			} else {
				mbutton (mx,my);
			}
			if (global_editmode == false) {
				#ifndef __CX16__
				cputcxy (2,16,"new hs");
				cputcxy (2,17,"saving");
				#endif
				savehs ();
				#ifndef __CX16__
				cclearxy (2,16,6); // clear the 'new hs' text.
				cclearxy (2,17,6); // clear the 'saving' text.
				#endif
			}
			cputsxy (2,16,"ready");
		}
	}
	return 0;
}
