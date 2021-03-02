#ifndef BOARD_H
#define BOARD_H

// exported functions
extern void getboard ();
extern void stashboard ();
extern void fetchboard ();
extern void updatescore ();
extern void updateboard ();

// exported vars
extern char board[16][16];

// local functions
void drawgameboard ();
void newrandboard ();

#endif
