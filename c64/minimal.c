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
#include <peekpoke.h>

#include <joystick.h>

#define LX  12	// low 'x' val (left side of playfield)
#define HX  27	// high 'x' val (right side of playfield)
#define LY  1	// low 'y' val (top of playfield)
#define HY  16	// high 'y' val (bottom of playfield)

char board[16][16];
short jx,jy;
bool nrb;
bool b;

typedef struct Queue {
	unsigned int capacity;
	unsigned int size;
	unsigned int front;
	int rear;
	unsigned char *qx;
	unsigned char *qy;
}Queue;

unsigned char dirtable[4][2] = {
	{ 117 , 106 },
	{ 105 , 107 },
	{ 117 , 105 },
	{ 107 , 106 }
};

void vsyncw (unsigned char c) {
	unsigned char x;

	for (x = 1; x <= c; ++x) {
		waitvsync ();
	};
}


Queue * createQueue (unsigned int maxElements) {
	Queue *Q;
	Q = (Queue *)malloc (sizeof (Queue));

	Q->qx = (unsigned char *)malloc (sizeof (unsigned char)*maxElements);
	Q->qy = (unsigned char *)malloc (sizeof (unsigned char)*maxElements);
	Q->size = 0;
	Q->capacity = maxElements;
	Q->front = 0;
	Q->rear = -1;

	return Q;
}

void Deqeue (Queue *Q) {
	if (Q->size == 0) {
		return;
	} else {
		--Q->size;
		++Q->front;
		if (Q->front == Q->capacity) {
			Q->front=0;
		}
	}
}

void Enqueue (Queue *Q,unsigned char ex, unsigned char ey) {
	// if the queue is full, we cannot push an element into it as there is no space for it
	if (Q->size == Q->capacity) {
		gotoxy (29,18);
		cprintf ("queue is full");
		cgetc ();
		exit (1);
	} else {
		++Q->size;
		++Q->rear;
		// Should probably just make the Q bigger, this makes the Q circular
		// Pretty sure this is one of the places things go goofy on the C-64 version
		if (Q->rear == Q->capacity) {
			Q->rear = 0;
		}
		Q->qx[Q->rear] = ex;
		Q->qy[Q->rear] = ey;
	}
	return;
}

void check (Queue *Q, unsigned char cx, unsigned char cy, unsigned char dir) {
	unsigned char c,cc;

	switch (dir) {
		case 0:                    // left
			if (cx > LX) {
				--cx;
				break;
			} else {
				return;
			}
		case 1:                    // right
			if (cx < HX) {
				++cx;
				break;
			} else {
				return;
			}
		case 2:                    // up
			if (cy > LY) {
				--cy;
				break;
			} else {
				return;
			}
		case 3:                    // down
			if (cy < HY) {
				++cy;
				break;
			} else {
				return;
			}
		default:
			gotoxy (29,24);
			cprintf ("what?!");
			exit (1);
	}

	// check to see if we're already marked to rotate
	gotoxy (cx,cy);
	cc = cpeekcolor ();

	if (cc == COLOR_RED) {
		return;
	}

	c = cpeekc ();

	if (c == dirtable[dir][0] || c == dirtable[dir][1]) {
		textcolor (COLOR_RED);
		cputc (c);
		textcolor (COLOR_WHITE);

		Enqueue (Q,cx,cy);
	}
}

void processQ (Queue *Q) {
	static unsigned char c,nc,hc;
	static unsigned char mx,my;

	while ((unsigned char) Q->size != 0) {
		mx = (unsigned char) Q->qx[Q->front];
		my = (unsigned char) Q->qy[Q->front];

		gotoxy (mx,my);

		c = cpeekc ();

		switch (c) {
			case 105:
				hc = 101;
				nc = 107;
				check (Q,mx,my,2); // up
				check (Q,mx,my,0); // left
				break;
			case 106:
				hc = 103;
				nc = 117;
				check (Q,mx,my,3); // down
				check (Q,mx,my,1); // right
				break;
			case 107:
				hc = 102;
				nc = 106;
				check (Q,mx,my,2); // up
				check (Q,mx,my,1); // right
				break;
			case 117:
				hc = 100;
				nc = 105;
				check (Q,mx,my,3); // down
				check (Q,mx,my,0); // left
				break;
		}

		textcolor (COLOR_WHITE);
		vsyncw (3);
		cputcxy (mx,my,nc);

		Deqeue (Q);

		//vsyncw (1);
	}
}


void joyinfo() {
	unsigned char J;
	unsigned char oc,ojx,ojy;
	
	gotoxy (jx,jy);
	oc = cpeekc ();
	cputc (118);
	
	while (!b) {
		ojx = jx;
		ojy = jy;
		
		J = joy_read (JOY_2);
		// gotoxy (0,0);
		// cprintf("j=%d\r\n",J);
 		switch (J) {
			case 1:      // up
				--jy;
				break;
			case 2:      // down
				++jy;
				break;
			case 4:      // left
				--jx;
				break;
			case 5:      // up left
				--jx;
				--jy;
				break;
			case 6:      // down left
				--jx;
				++jy;
				break;
			case 8:      // right 
				++jx;
				break;
			case 9:      // up right
				++jx;
				--jy;
				break;
			case 10:     // down right
				++jx;
				++jy;
				break;
			case 16:     // button
				cputcxy (jx,jy,oc);
				b = true;
				return;
		}

		if (jy <= 0 ) jy = 0;
		if (jy >= 24 ) jy = 24;
		if (jx <=0 ) jx = 0;
		if (jx >= 39 ) jx = 39;
		
		cputcxy (ojx,ojy,oc);
		gotoxy (jx,jy);
		oc = cpeekc();
		cputc (118);
		vsyncw (3);
	}
	return;
}

void drawgameboard () {
	unsigned char x,y;

	for (y = 0; y <= 15; ++y) {
		for (x = 0; x <= 15; ++x) {
			cputcxy(x + LX, y + LY, board[x][y]);
		}
	}
}

void newrandboard () {
	unsigned char c,x,y;
	unsigned char cpart;

	for (y = 0; y <= 15 ; ++y) {
		for (x = 0; x <= 15 ; ++x) {
			c = (rand () % 4);
			switch (c) {
				case 0:
					cpart = 105;
					break;
				case 1:
					cpart = 106;
					break;
				case 2:
					cpart = 107;
					break;
				case 3:
					cpart = 117;
					break;
			}
			board[x][y] = cpart;
		}
	}
}

void cbox (unsigned char tx,unsigned char ty,unsigned char bx,unsigned char by) {
	unsigned char bw,bh;
	unsigned char hlc,vlc; // horizontal/vertical line drawing char
	unsigned char x;

	bw = (bx - tx) - 1;
	bh = (by - ty) - 1;

	hlc = 96;  // shifted 'C'
	vlc = 98;  // shifted 'B'

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

	cbox (bx,by,bx+bl+1,by+2);
	cputsxy (bx+1,by+1,btext);
}

void updateboard () {
	cbox (LX-1,LY-1,HX+1,HY+1);

	if (nrb) {
		newrandboard ();
	}

	drawgameboard ();
	drawbutton (31,5,"reset");  // generate new random board

}

void mbutton (unsigned char mx, unsigned char my) {
	// clicked on game 'reset' button
	if ((mx >= 31 && mx <= 37) && (my >= 5 && my <= 7)) {
		nrb = true;
		clrscr ();
		updateboard ();
	}
}


int main () {
	unsigned char c,mx,my;

	Queue *Q = createQueue (63);

	clrscr ();
	// kick us back into upper-case/PETSCII mode
	//__asm__ ("lda #$8E");
	//__asm__ ("jsr $FFD2");
	POKE (53272,21);

	textcolor (COLOR_WHITE);
	bordercolor (COLOR_BLUE);
	bgcolor (COLOR_BLUE);
	joy_install (joy_static_stddrv);

	_randomize ();

	nrb = true;
	updateboard ();

	while (1) {
		jx = 20;
		jy = 9;

		joyinfo ();

		mx = (unsigned char) jx;
		my = (unsigned char) jy;

		vsyncw (5);
		
		// check for clicks on gameboard
		if (b) {
			if ((mx >= LX && mx <= HX) && (my >= LY && my <= HY)) {
				cclearxy (2,16,5);  // clear the 'ready' text
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
		b = false;
		cputsxy (2,16,"ready");
	}
	return 0;
}
