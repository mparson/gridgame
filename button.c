#include <conio.h>
#include <stdbool.h>

#include "button.h"
#include "global.h"
#include "board.h"
#include "boardedit.h"

bool global_editmode;
bool global_newrandboard;

void mbutton (unsigned char mx, unsigned char my) {
	if (global_editmode) {
		// 'load' button
		if ((mx >= 31 && mx <= 37) && (my >= 11 && my <= 13)) {
			loadboard ();
		}
		// 'save' button
		if ((mx >= 31 && mx <= 37) && (my >= 14 && my <= 17)) {
			saveboard ();
		}
		// high score 'reset' button
		if ((mx >= 1 && mx <= 7) && (my >=11 && my <= 13)) {
			global_nhs = true;
			global_hscore = 0;
			savehs ();
		}
		// mark column same as top/bottom cell
		if ((mx >= 12 && mx <= 27) && (my == 0 || my == 17)) {
			markcol (mx,my);
		}
		// mark row same as left/right
		if ((mx == 11 || mx == 28) && (my >= 1 && my <= 16)) {
			markrow (mx,my);
		}
	} 
	// clicked on game 'reset' button
	if ((mx >= 31 && mx <= 37) && (my >= 5 && my <= 7)) {
		global_newrandboard = true;
		clrscr ();
		updateboard ();
	}
	// clicked on 'done/edit' button
	else if ((mx >= 31 && mx <= 37) && (my >= 8 && my <= 10)) {
		// 'done' button
		if (global_editmode) {
			global_editmode = false;
			global_score = 0;
			getboard ();
			updateboard ();
		} else {
		// 'edit' button
			getboard ();
			global_editmode = true;
			global_newrandboard = false;
			updateboard ();
		}
	}
	// clicked on the 'replay' button
	else if ((mx >= 30 && mx <= 37) && (my >= 15 && my <= 17)) {
		fetchboard ();
		global_newrandboard = false;
		updateboard ();
	}
}
