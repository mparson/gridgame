#include <conio.h>

#include "boardedit.h"

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
