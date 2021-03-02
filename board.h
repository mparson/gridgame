#ifndef BOARD_H
#define BOARD_H

// exported functions
extern void getboard ();
extern void updatescore ();
extern void updateboard ();

#ifdef __CX16__
extern void stashboard ();
extern void fetchboard ();
#endif

// exported vars
extern char board[16][16];

// local functions
void drawgameboard ();
void newrandboard ();

#endif
