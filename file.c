#include <stdio.h>
#include <conio.h>
#include <cbm.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "global.h"
#include "file.h"
#include "draw.h"
#include "board.h"

unsigned int global_score;
unsigned int global_hscore;
bool global_overwrite;
//bool global_nhs;

unsigned char filename[16]; // 16 char filename limit in C= DOS
unsigned char tmpname[20];  // +3 for "S0:" and +4 for ",s,[rw]"
unsigned char fbuf[20]; // should only need 16, but this gives us some breathing room.

char board[16][16];
bool sb;

void loadhs () {
	global_overwrite = true;
	loadfile ("highscore",6);  // high scores are max 6 chars long.
	global_hscore = atoi (fbuf);
}

void savehs () {
	if (global_editmode) {
		global_nhs = true;
		global_hscore = 0;
		updatescore ();
	}
	
	if (global_nhs == true) {
		global_overwrite = true;
		gotoxy (0,24);
		itoa (global_hscore,fbuf,10);
		gotoxy (0,24);
		savefile ("highscore",strlen (fbuf));
		global_nhs = false;
	}
}

// TODO: handle non-existing file for board loading
void loadfile (unsigned char filename[], unsigned char fsize) {
	unsigned char y;
	
	if (global_overwrite) {
		strcpy (tmpname,filename);
		strcat (tmpname,",s,r");

		cbm_open (2,8,2,tmpname);
		
		if (sb) {
			for (y = 0; y <= 15; ++y) {
				fbuf[0] = '\0';
				gotoxy (LX,LY + y);
				cbm_read (2,fbuf,16); // only read 16 chars
				cprintf ("%s",fbuf);
			}
			sb = 0;
		} else { 
			cbm_read (2,fbuf,fsize);
			cbm_close (2);
		}
	}
}

static void append (char* s,char c) {
	unsigned char len;

	len = strlen(s);
	
	s[len] = c;
	s[len + 1] = '\0';
}

// TODO: handle existing file for board saving
void savefile (unsigned char filename[], unsigned int fsize) {
	unsigned char x,y;
	
	if (global_overwrite) {
		strcpy (tmpname,"s0:");
		strcat (tmpname,filename);
		strcat (tmpname,",s");
		
		cbm_open (2,8,15,tmpname);
		cbm_close (2);
	}

	strcpy (tmpname,filename);
	strcat (tmpname,",s,w");
	cbm_open (2,8,2,tmpname);
	
	if (sb) {
		for (y = 0; y <= 15; ++y) {
			fbuf[0] = '\0';
			for (x = 0; x <= 15; ++x) {
				append (fbuf,board[x][y]);
			}
			// cprintf ("%s\r\n",fbuf);
			cbm_write (2,fbuf,16); // only write 16 chars
		}
		sb = 0;
	} else {
		cbm_write (2,fbuf,fsize);
	}

	cbm_close (2);
}

void loadboard () {
	unsigned char c;
	
	cursor (1);
	cboxclear (0,18,39,24);  // clear the bottom of the screen}
	cputsxy (0,18,"filename? ");
	scanf ("%s",&filename);
	
	gotoxy (0,19);
	cprintf ("loading board: %s",filename);
	global_overwrite = true; // force attempting to load (TODO: why?)
	sb = 1;
	loadfile (filename,1);   // board loading does it's own file sizing
	cclearxy (0,21,40);      // clear 'load...' text.

	gotoxy (0,21);
	cprintf ("board %s loaded\r\n",filename);
	// wait for 2 seconds
	for (c = 0; c <= 120 ; ++c) { 
		waitvsync ();
	}
	cboxclear (0,18,39,24);  // clear the bottom of the screen}
	cursor (0);
	updateboard ();
}

void saveboard () {
	unsigned char x,y;
	unsigned char c;

	cursor (1);
	cputsxy (0,18,"filename? ");
	scanf ("%s",&filename);
	
	for (y = 0; y <= 15; ++y) {
		for (x = 0; x <= 15; ++x) {
			gotoxy (x + LX,y + LY);
			board[x][y] = cpeekc ();
		}
	}
	
	gotoxy (0,19);
	cprintf ("saving board: %s",filename);
	// force attempting to save
	global_overwrite = true;
	// so savefile () knows to do the board handling
	sb = 1;
	savefile (filename,1);
	// clear 'saving...' text.
	cclearxy (0,21,40);
	gotoxy (0,21);
	cprintf ("board %s saved\r\n",filename);
	// wait for 2 seconds
	for (c = 0; c <= 120 ; ++c) { 
		waitvsync ();
	}
	// clear the bottom of the screen
	cboxclear (0,18,39,24); 
	cursor (0);
	updateboard ();
}

