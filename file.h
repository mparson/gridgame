#ifndef FILE_H
#define FILE_H

#include <stdbool.h>

// exported stuff

extern void loadfile (unsigned char filename[], unsigned char fsize);
extern void savefile (unsigned char filename[], unsigned int fsize);
extern void loadboard ();
extern void saveboard ();
extern bool sb;   // originally "save board", but is used by both {load,save}file to trigger board handling
extern unsigned char filename[16]; // 16 char filenames
extern unsigned char fbuf[20]; // should only need 16, but this gives us some breathing room.
extern void loadhs ();
extern void savehs ();

// local stuff


#endif
