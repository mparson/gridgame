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

// Some colors we use in here
#define WHITE 97
#define RED   98

unsigned char dirtable[4][2] = {
	{ 117 , 106 },
	{ 105 , 107 },
	{ 117 , 105 },
	{ 107 , 106 }
};

typedef struct Queue {
	unsigned int capacity;
	unsigned int size;
	unsigned int front;
	int rear;
	unsigned char *qx;
	unsigned char *qy;
}Queue;

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

void Deqeue (Queue *Q){
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

	if (global_editmode) {
		return;
	}
	
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
		
	if (cc == RED) {
		return;
	}

	c = cpeekc ();

	if (c == dirtable[dir][0] || c == dirtable[dir][1]) {
		textcolor (RED);
		cputc (c);
		textcolor (WHITE);
		
		Enqueue (Q,cx,cy);
		++global_score;
		updatescore ();
	}
}

void processQ(Queue *Q) {
	static unsigned char c,nc;
	static unsigned char mx,my;
	
	while ((unsigned char) Q->size != 0) {
		mx = (unsigned char) Q->qx[Q->front];
		my = (unsigned char) Q->qy[Q->front];
		
		gotoxy (mx,my);
		
		c = cpeekc ();

		switch (c) {
			case 105:
				nc = 107;
				check (Q,mx,my,2); // up
				check (Q,mx,my,0); // left
				break;
			case 106:
				nc = 117;
				check (Q,mx,my,3); // down
				check (Q,mx,my,1); // right
				break;
			case 107:
				nc = 106;
				check (Q,mx,my,2); // up
				check (Q,mx,my,1); // right
				break;
			case 117:
				nc = 105;
				check (Q,mx,my,3); // down
				check (Q,mx,my,0); // left
				break;
		}

		textcolor (WHITE);
		cputcxy (mx,my,nc);

		Deqeue (Q);

		for (c = 0;  c <= 1; ++c) {
			waitvsync ();
		}
	}
	cputsxy (2,16,"ready");
}

int main () {
	struct mouse_info info;
	unsigned char c,mbl,mx,my;

	Queue *Q = createQueue (127);

	videomode (0);

	clrscr ();

	// kick us back into upper-case/PETSCII mode
	cbm_k_bsout (CH_FONT_UPPER);

	#ifndef __CX16__
	textcolor (WHITE);
	#endif

	_randomize ();

	mouse_load_driver (&mouse_def_callbacks, mouse_stddrv);
	mouse_install (&mouse_def_callbacks,mouse_static_stddrv);
	mouse_show ();

	loadhs ();
	global_redrawboard = true;
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
				textcolor (RED);
				cputcxy (c,mx,my);
				textcolor (WHITE);
				Enqueue (Q,mx,my);
				processQ (Q);
			} else {
				mbutton (mx,my);
			}
			if (global_editmode == 0) {
				savehs ();
			}
		}
	}
	return 0;
}