#ifndef FILE_H
#define FILE_H

#include <stdbool.h>

// local functions
void prepend (char* s, const char* t);
void append (char* s,char c);

// exported functions
extern void loadfile (unsigned char filename[], unsigned char fsize);
extern void savefile (unsigned char filename[], unsigned int fsize);
extern void loadhs ();
extern void savehs ();

// exported vars
extern bool sb;   // originally "save board", but is used by both {load,save}file to trigger board handling
extern unsigned char filename[16]; // 16 char filenames
extern unsigned char fbuf[20]; // should only need 16, but this gives us some breathing room.

#endif
