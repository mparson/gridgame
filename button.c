#include <conio.h>
#include <stdbool.h>

#include "button.h"
#include "global.h"
#include "board.h"
#include "boardedit.h"

bool global_editmode;
bool global_redrawboard;

void mbutton (unsigned char mx, unsigned char my) {
	// clicked on game 'reset' button
	if ((mx >= 31 && mx <= 37) && (my >= 5 && my <= 7)) {
		global_redrawboard = true;
		clrscr ();
		updateboard ();
	} 
	// clicked on 'done/edit' button
	else if ((mx >= 31 && mx <= 37) && (my >= 8 && my <= 10)) {
		if (global_editmode) {
			global_editmode = false;
			global_redrawboard = false;
			global_score = 0;
			updateboard ();
		} else {
			global_editmode = true;
			global_redrawboard = false;
			updateboard ();
		}
	}
	if (global_editmode) {
		if ((mx >= 31 && mx <= 37) && (my >= 11 && my <= 13)) {
			loadboard ();
		}
		if ((mx >= 31 && mx <= 37) && (my >= 14 && my <= 17)) {
			saveboard ();
		}
		if ((mx >= 1 && mx <= 7) && (my >=11 && my <= 13)) {
			global_nhs = true;
			global_hscore = 0;
			savehs ();
		}
		if ((mx >= 12 && mx <= 27) && (my == 0 || my == 17)) {
			// mark column same as top/bottom cell
			markcol (mx,my);
		}
		if ((mx == 11 || mx == 28) && (my >= 1 && my <= 16)) {
			// mark row same as left/right
			markrow (mx,my);
		}
	}
}
