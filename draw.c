#include <conio.h>
#include <string.h>

#include "global.h"
#include "draw.h"

void cboxclear (unsigned char tx,unsigned char ty,unsigned char bx,unsigned char by) {
	unsigned char bw,bh;
	unsigned char y;

	bw = (bx - tx) + 1;
	bh = (by - ty) + 1;

	for (y = ty; y <= by; ++y) {
		cclearxy (tx,y,bw);
	}
}

void cbox (unsigned char tx,unsigned char ty,unsigned char bx,unsigned char by) {
	unsigned char bw,bh;
	unsigned char hlc,vlc; // horizontal/vertical line drawing char
	unsigned char x;

	bw = (bx - tx) - 1;
	bh = (by - ty) - 1;

	if (global_editmode) {
		hlc = 42;  // '*'
		vlc = hlc;
	} else {
		hlc = 96;  // shifted 'C'
		vlc = 98;  // shifted 'B'
	}

	cputcxy (tx,ty,117);   // TL corner
	cputcxy (bx,ty,105);   // TR corner
	cputcxy (bx,by,107);   // BR corner
	cputcxy (tx,by,106);   // BL corner

	for (x = 1; x <= bw; ++x) {
		cputcxy (tx+x,ty,hlc);
		cputcxy (tx+x,by,hlc);
	}

	for (x = 1; x <= bh; ++x) {
		cputcxy (tx,ty+x,vlc);
		cputcxy (bx,ty+x,vlc);
	}
}

void drawbutton (char bx, char by, char btext[]) {
	unsigned char bl = strlen (btext);
	bool em = global_editmode; // save the global_editmode

	global_editmode = false; // we want real line draw chars, not '*'

	cbox (bx,by,bx+bl+1,by+2);
	cputsxy (bx+1,by+1,btext);

	global_editmode = em; // back to whatever it was before
}
