#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "board.h"
#include "draw.h"
#include "boardedit.h"
#include "file.h"

void markcol (unsigned char mx,unsigned char my) {
	unsigned char c,y;

	// clicked on top column marker
	if (my == 0) {
		gotoxy (mx,1);
		c = cpeekc ();
	}
	// clicked on bottom column marker
	else if (my == 17) {
		gotoxy (mx,16);
		c = cpeekc ();
	}
	for (y = 1; y <= 16; ++y) {
		cputcxy (mx,y,c);
	}
}

void markrow (unsigned char mx,unsigned char my) {
	unsigned char c,x;

	// clicked on top column marker
	if (mx == 11) {
		gotoxy (12,my);
		c = cpeekc ();
	}
	// clicked on bottom column marker
	else if (mx == 28) {
		gotoxy (27,my);
		c = cpeekc ();
	}
	for (x = 12; x <= 27; ++x) {
		cputcxy (x,my,c);
	}
}

void loadboard () {
	cursor (1);
	cboxclear (0,18,39,24);  // clear the bottom of the screen}
	cputsxy (0,18,"filename? ");
	scanf ("%s",&filename);

	gotoxy (0,19);
	cprintf ("loading board: %s",filename);
	sb = true;
	loadfile (filename,256); // board is 256 chars

	cclearxy (0,21,40);      // clear 'load...' text.

	gotoxy (0,21);
	cprintf ("board %s loaded\r\n",filename);
	// wait for 2 seconds
	vsyncw (120);
	cboxclear (0,18,39,24);  // clear the bottom of the screen}
	cursor (0);
	updateboard ();
}

void saveboard () {
	cursor (1);
	cboxclear (0,18,39,24);  // clear the bottom of the screen}
	cputsxy (0,18,"filename? ");
	scanf ("%s",&filename);

	if (strcmp (filename,(unsigned char*) "highscore")) {

		gotoxy (0,19);
		cprintf ("saving board: %s",filename);

		sb = true;
		savefile (filename,1);

		// clear 'saving...' text.
		cclearxy (0,21,40);
		gotoxy (0,21);
		cprintf ("board %s saved\r\n",filename);
		// wait for 2 seconds
		vsyncw (120);
		// clear the bottom of the screen
		cboxclear (0,18,39,24);
		cursor (0);
		cclearxy (0,16,7);
		getboard ();
		updateboard ();
	} else {
		cputsxy (0,19,"can't name a board 'highscore'");
		vsyncw (120);
		return;
	}
}