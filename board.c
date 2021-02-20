#include <conio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "board.h"
#include "draw.h"

static unsigned char grid[4][8] = {
	{ 117,96,105,160,160,160,160,98 },
	{ 98,160,160,160,160,113,160,98 },
	{ 98,96,105,117,96,98,117,179 },
	{ 106,96,107,98,160,98,106,107 }
};

static unsigned char game[4][9] = {
	{ 117,105,117,105,160,160,160,117,105 },
	{ 106,179,117,179,117,178,105,171,107 },
	{ 160,98,106,107,98,98,98,106,107 },
	{ 106,107,160,160,160,160,160,160,160 }
};

bool global_nhs;

void fetchboard () {
	unsigned char x,y;

	for (y = 0; y <= 15; ++y) {
		for (x = 0; x <= 15; ++x) {
			gotoxy (x + LX,y + LY);
			board[x][y] = cpeekc ();
		}
	}
}

void drawgameboard () {
	unsigned char x,y;

	for (y = 0; y <= 15; ++y) {
		for (x = 0; x <= 15; ++x) {
			cputcxy(x + LX, y + LY, board[x][y]);
		}
	}
}

void stashboard () {
	fetchboard ();
	memcpy ((void*) 0xA000, board, 256);
}

void getboard () {
	memcpy (board, (void*) 0xA000, 256);
}

void updatescore () {
	if (global_score >= global_hscore) {
		global_nhs = true;
		global_hscore = global_score;
	}
	if (global_hscore || global_nhs == true) {
		gotoxy (2,10);
		cprintf ("%6d",global_hscore);
	}

	gotoxy (2,6);
	cprintf ("%6d",global_score);
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

void updateboard () {
	unsigned char row,rows,cols;

	// print the "Grid" text
	row = 0;
	gotoxy (2,row);
	for (rows = 0; rows < 4; ++rows) {
		for (cols = 0; cols < 8; ++cols) {
			cprintf ("%c",grid[rows][cols]);
		}
		row++;
		gotoxy (2,row);
	}

	// print the "game" text
	row = 0;
	gotoxy (29,row);
	for (rows = 0; rows < 4; ++rows) {
		for (cols = 0; cols < 9; ++cols) {
			cprintf ("%c",game[rows][cols]);
		}
		row++;
		gotoxy (29,row);
	}

	cbox (LX-1,LY-1,HX+1,HY+1);

	if (global_newrandboard) {
		newrandboard ();
	}

	drawgameboard ();

	cputsxy (2,5,"score:");
	cputsxy (2,8,"high");
	cputsxy (2,9,"score:");

	updatescore ();

	if (global_editmode) {
		cboxclear (31,5,37,7);      // erase 'reset' button
		cboxclear (0,18,39,24);     // erase the bottom of screen
		cboxclear (29,11,39,17);    // erase everything below the 'edit' button.
		drawbutton (31,8," done");  // want 'reset' and 'done' buttons to be same size
		drawbutton (31,11," load"); // load new board
		drawbutton (31,14," save"); // save board to disk
		drawbutton (1,11,"reset");  // high score reset button
		gotoxy(0,19);
		cprintf ("click on a piece to rotate it\r\n");
		cprintf ("clicking on an '*' on the border will\r\n");
		cprintf ("change the whole row or column to match\r\n");
		cprintf ("the adjacent piece.");
	} else {
		cboxclear (31,11,37,16);    // remove the load/save buttons above
		cboxclear (1,11,7,13);      // remove the 'reset' button under the high score
		drawbutton (31,5,"reset");  // generate new random board
		drawbutton (31,8," edit");  // edit board
		drawbutton (30,15,"replay");// play original board again

		cboxclear (0,18,39,24);     // erase the bottom of screen
		gotoxy (0,19);
		cprintf ("the objective of the game is to get a\r\n");
		cprintf ("chain reaction of tiles as long as\r\n");
		cprintf ("possible.tiles will start each other if\r\n");
		cprintf ("their lines are connected.\r\n");
		cprintf ("click on a tile to start.");
	}
}
