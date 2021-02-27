#include <cbm.h>

#include "global.h"

void vsyncw (unsigned char c) {
	unsigned char x;
	
	for (x = 1; x <= c; ++x) {
		waitvsync ();
	};
}