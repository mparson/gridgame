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

unsigned char filename[16]; // 16 char filename limit in C= DOS
unsigned char tmpname[20];  // +3 for "S0:" and +4 for ",s,[rw]"
unsigned char fbuf[20]; // should only need 16, but this gives us some breathing room.

char board[16][16];
bool sb;

void loadhs () {
	loadfile ((unsigned char*) "highscore",6);  // high scores are max 6 chars long.
	global_hscore = atoi ((const char*) fbuf);
}

void savehs () {
	if (global_editmode) {
		updatescore ();
	}

	if (global_nhs == true) {
		itoa (global_hscore,(char*) fbuf,10);
		while (strlen ((const char*) fbuf) < 6) {
			prepend ((char*) fbuf,"0");
		}
		savefile ((unsigned char*) "highscore",strlen((const char*) fbuf));
		global_nhs = false;
	}
}

// TODO: handle non-existing file for board loading
void loadfile (unsigned char filename[], unsigned char fsize) {
	unsigned char x,y;

	strcpy ((char*) tmpname,(const char*) filename);
	strcat ((char*) tmpname,",s,r");

	cbm_open (2,8,2,(const char*) tmpname);

	if (sb) {
		for (y = 0; y <= 15; ++y) {
			for (x = 0; x <= 15; ++x) {
				fbuf[0] = '\0';
				cbm_read (2,fbuf,1);
				board[x][y] = fbuf[0];
			}
		}
		sb = false;
	} else {
		cbm_read (2,fbuf,fsize);
	}
	cbm_close (2);
}

void prepend (char* s, const char* t) {
	unsigned char len = strlen(t);

	memmove(s + len, s, strlen(s) + 1);
	memcpy(s, t, len);
}

void append (char* s,char c) {
	unsigned char len;

	len = strlen(s);

	s[len] = c;
	s[len + 1] = '\0';
}

// TODO: handle existing file for board saving
void savefile (unsigned char filename[], unsigned int fsize) {
	unsigned char x,y;

	strcpy ((char*) tmpname,"s0:");
	strcat ((char*) tmpname,(const char*) filename);
	strcat ((char*) tmpname,",s");

	cbm_open (2,8,15,(const char*) tmpname);
	cbm_close (2);

	strcpy ((char*) tmpname,(const char*) filename);
	strcat ((char*) tmpname,",s,w");
	cbm_open (2,8,2,(char*) tmpname);

	if (sb) {
		getboard ();
		for (y = 0; y <= 15; ++y) {
			fbuf[0] = '\0';
			for (x = 0; x <= 15; ++x) {
				append ((char*) fbuf,board[x][y]);
			}

			cbm_write (2,fbuf,16); // only write 16 chars
		}
		sb = false;
	} else {
		cbm_write (2,fbuf,fsize);
	}
	cbm_close (2);
}
