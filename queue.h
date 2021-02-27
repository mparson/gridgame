#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

#include "board.h"

#define CHARSTART 63488

// exported struct
typedef struct Queue {
	unsigned int capacity;
	unsigned int size;
	unsigned int front;
	int rear;
	unsigned char *qx;
	unsigned char *qy;
}Queue;

// exported functions
extern void setupchars ();
extern void Enqueue (Queue *Q,unsigned char ex, unsigned char ey);
extern void processQ (Queue *Q);

// local functions
Queue * createQueue (unsigned int maxElements);
void Deqeue (Queue *Q);
void check (Queue *Q, unsigned char cx, unsigned char cy, unsigned char dir);

#endif
