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
 may or may not occur before, during, or after aquisition of this
 software.

 Play at your own risk, no lifeguard on duty.
 */
 
#include <conio.h>
#include <stdlib.h>
#include <mouse.h>
#include <string.h>
#include <stdbool.h>

#define LX  12	// low 'x' val (left side of playfield)
#define HX  27	// high 'x' val (right side of playfield)
#define LY  1	// low 'y' val (top of playfield)
#define HY  16	// high 'y' val (bottom of playfield)

// Some colors we use in here
#define WHITE 97
#define RED   98

static char rbuf[255];
static char wbuf[255];
unsigned int hscore;
bool nhs = false;

unsigned int score = 0;

unsigned char grid[4][8] = {
	{ 117,96,105,160,160,160,160,98 },
	{ 98,160,160,160,160,113,160,98 },
	{ 98,96,105,117,96,98,117,179 },
	{ 106,96,107,98,160,98,106,107 }
};

unsigned char game[4][9] = {
	{ 117,105,117,105,160,160,160,117,105 },
	{ 106,179,117,179,117,178,105,171,107 },
	{ 160,98,106,107,98,98,98,106,107 },
	{ 106,107,160,160,160,160,160,160,160 }
};

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

Queue * createQueue(unsigned int maxElements) {
	Queue *Q;
	Q = (Queue *)malloc(sizeof(Queue));

	Q->qx = (unsigned char *)malloc(sizeof(unsigned char)*maxElements);
	Q->qy = (unsigned char *)malloc(sizeof(unsigned char)*maxElements);
	Q->size = 0;
	Q->capacity = maxElements;
	Q->front = 0;
	Q->rear = -1;

	return Q;
}

unsigned int Deqeue(Queue *Q){
	if (Q->size == 0) {
		return 0;
	} else {
		Q->size--;
		Q->front++;
		if (Q->front == Q->capacity) {
			Q->front=0;
		}
	}
	return (unsigned int) Q->size;
}

void Enqueue(Queue *Q,unsigned char ex, unsigned char ey) {
	// if the queue is full, we cannot push an element into it as there is no space for it
	if (Q->size == Q->capacity) {
		gotoxy(29,18);
		cprintf("queue is full");
		cgetc();
		exit(1);
	} else {
		Q->size++;
		Q->rear++;
		// Should probably just make the Q bigger, this makes the Q circular
		if (Q->rear == Q->capacity) {
			Q->rear = 0;
		}

		Q->qx[Q->rear] = ex;
		Q->qy[Q->rear] = ey;
	}
	return;
}

void updatescore() {
	if (score >= hscore) {
		nhs = true;
		hscore = score;
	}
	if (hscore) {
		gotoxy(2,10);
		cprintf("%6d",hscore);
	}
	
	gotoxy(2,6);
	cprintf("%6d",score);
}

void loadhs() {
	cbm_open(2,8,2,"highscore,s");
	cbm_read(2,rbuf,6);
	cbm_close(2);
	hscore = atoi(rbuf);
}

void savehs() {
	//unsigned char retval;
	
	if (nhs == true) {
		cbm_open(2,8,15,"s0:highscore,s");
		cbm_close(2);

		cbm_open(2,8,2,"highscore,s,w");
		
		itoa(hscore,wbuf,10);
		cbm_write(2,wbuf,strlen(wbuf));
		cbm_close(2);

		nhs = false;
	}
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
			gotoxy(29,24);
			cprintf ("what?!");
			exit(1);
	}
	
	gotoxy(cx,cy);

	// check to see if we're already marked to rotate
	cc = cpeekcolor();
		
	if (cc == RED) {
		return;
	}

	c = cpeekc();

	if (c == dirtable[dir][0] || c == dirtable[dir][1]) {
		textcolor(RED);
		cputc(c);
		textcolor(WHITE);
		
		Enqueue(Q,cx,cy);
		++score;
		updatescore();
	}
}

void processQ(Queue *Q) {
	static unsigned char c,nc;
	static unsigned char mx,my;

	unsigned char mq = 0;
	
	while ((unsigned char) Q->size != 0) {
		mx = (unsigned char) Q->qx[Q->front];
		my = (unsigned char) Q->qy[Q->front];
		
		gotoxy(mx,my);
		
		c = cpeekc();

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

		textcolor(WHITE);
		cputcxy(mx,my,nc);

		if ((char)Q->size > mq) {
			mq = (char)Q->size;
		}

		Deqeue(Q);

		for (c = 0;  c <= 1; ++c) {
			waitvsync();
		}
	}
	cputsxy(2,16,"ready");
}

void cbox (unsigned char tx,unsigned char ty,unsigned char bx,unsigned char by) {
	unsigned char bw,bh;
	
	bw = (bx - tx) - 1;
	bh = (by - ty) - 1;
	
	cputcxy(tx,ty,117);   // TL corner
	chlinexy(tx+1,ty,bw); // top line
	cputcxy(bx,ty,105);   // TR corner
	cvlinexy(bx,ty+1,bh); // right line
	cputcxy(bx,by,107);   // BR corner
	chlinexy(tx+1,by,bw); // bottom line
	cputcxy(tx,by,106);   // BL corner
	cvlinexy(tx,ty+1,bh); // left line
}

void newboard() {
	unsigned char row,rows,cols,x;
	unsigned char cpart;

	// print the "Grid" text
	row = 0;
	gotoxy(2,row);
	for (rows = 0; rows < 4; rows++) {
		for (cols = 0; cols < 8; cols++) {
			cprintf("%c",grid[rows][cols]);
		}
		row++;
		gotoxy(2,row);
	}

	// print the "game" text
	row = 0;
	gotoxy(29,row);
	for (rows = 0; rows < 4; rows++) {
		for (cols = 0; cols < 9; cols++) {
			cprintf("%c",game[rows][cols]);
		}
		row++;
		gotoxy(29,row);
	}

	cbox(LX-1,LY-1,HX+1,HY+1);

	for (rows = LY; rows <= HY ; rows++) {
		for (cols = LX; cols <= HX ; cols++) {
			x = (rand() % 4);
			switch (x) {
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
			cputcxy(cols,rows,cpart);
		}
	}
	
	cputsxy(2,5,"score:");
	cputsxy(2,8,"high");
	cputsxy(2,9,"score:");
	
	updatescore();
	
	// reset button
	cputsxy(31,5,"UCCCCCI");
	cputsxy(31,6,"BresetB");
	cputsxy(31,7,"JCCCCCK");
	
	cputsxy(8,18,"click on a piece to start.");
	cputsxy(8,19,"try to get the chain reaction");
	cputsxy(8,20,"going as long as possible.");
	cputsxy(8,21,"edges touching trigger rotation.");
}

int main() {
	struct mouse_info info;
	unsigned char c,mbl,mx,my;

	Queue *Q = createQueue(127);

	videomode(0);

	clrscr();

	// kick us back into upper-case/PETSCII mode
	__asm__ ("lda #$8E");
	__asm__ ("jsr $FFD2");

	_randomize();

	mouse_load_driver(&mouse_def_callbacks, mouse_stddrv);
	mouse_install(&mouse_def_callbacks,mouse_static_stddrv);
	mouse_show();

	loadhs();
	
	newboard();
	
	while (1) {
		mouse_info(&info);
		mbl = (info.buttons & MOUSE_BTN_LEFT);
		for (c = 0; c <= 5; c++) {
			waitvsync();
		}
		if (mbl) {
			cclearxy(2,16,5);  // clear the 'ready' text 
			score = 0;
			updatescore();
			mx = info.pos.x >> 3;
			my = info.pos.y >> 3;
			// only process stuff on the board, certainly not the corners!
			if ((mx >= LX && mx <= HX) && (my >= LY && my <= HY)) {
				gotoxy(mx,my);
				c = cpeekc();
				textcolor(RED);
				cputcxy(c,mx,my);
				textcolor(WHITE);
				Enqueue(Q,mx,my);
				processQ(Q);
			} else {
				// clicked on 'reset' button
				if ((mx >= 31 && mx <= 37) && (my >= 5 && my <= 7)) {
					clrscr();
					newboard();
				}
			}
		}
		savehs();
	}
	return 0;
}