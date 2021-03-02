#include <conio.h>
#include <joystick.h>
#include <peekpoke.h>
#include <stdbool.h>

#include "joy.h"

short jx,jy;
bool global_b;

void joyinfo() {
	unsigned char J,n;
	unsigned char oc,ojx,ojy;
	
	gotoxy (jx,jy);
	oc = cpeekc ();
	cputc (118);
	
	while (!global_b) {
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
				global_b = true;
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
		for (n = 0; n <= 2; ++n) {
			waitvsync ();
		}
	}
	return;
}
