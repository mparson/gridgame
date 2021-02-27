#include <conio.h>
#include <stdlib.h>

// #include "board.h"
#include "global.h"
#include "queue.h"

unsigned char dirtable[4][2] = {
	{ 117 , 106 },
	{ 105 , 107 },
	{ 117 , 105 },
	{ 107 , 106 }
};

// screen display codes
unsigned char newchars[8] = { 68,69,70,71 };

// petscii codes
unsigned char newcharp[8] = { 100,101,102,103 };

// circle halfs
unsigned char ch[4][8] = {
	{ 0,0,0,126,255,195,129,129 },   // top
	{ 240,56,24,24,24,24,56,240 }, // right 
	{ 129,129,195,255,126,0,0,0 },   // bottom
	{ 15,28,24,24,24,24,28,15 }    // left
};

void setupchars () {
	unsigned char c,d,e;
	
	for (d = 0; d <= 3; ++d) {
		e = 0;
		for (c = 0; c <= 7; ++c) {
			vpoke (ch[d][c],CHARSTART + (newchars[d] * 8) + e);
			++e;
		}
	}
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

	if (cc == COLOR_RED) {
		return;
	}

	c = cpeekc ();

	if (c == dirtable[dir][0] || c == dirtable[dir][1]) {
		textcolor (COLOR_RED);
		cputc (c);
		textcolor (COLOR_WHITE);

		Enqueue (Q,cx,cy);
		++global_score;
		updatescore ();
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
		cputcxy (mx,my,hc);
		vsyncw (2);
		cputcxy (mx,my,nc);

		Deqeue (Q);

		vsyncw (1);
	}
}
