#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>;

#define LX  12	// low 'x' val (left side of playfield)
#define HX  27	// high 'x' val (right side of playfield)
#define LY  1	// low 'y' val (top of playfield)
#define HY  16	// high 'y' val (bottom of playfield)

// Some colors we use in here
#define WHITE 97
#define RED   98

extern unsigned int global_hscore;
extern unsigned int global_score;
extern bool global_overwrite;
extern bool global_nhs;
extern bool global_editmode;
extern bool global_redrawboard;

#endif
