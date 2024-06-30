#ifndef BUBBLE_STRUCT_INCLUDED
#define BUBBLE_STRUCT_INCLUDED
#include "game/phdvector.h"
typedef struct BUBBLE_STRUCT {
	PHD_VECTOR pos;
	short room_number;
	short speed;
	short size;
	short dsize;
	unsigned char shade;
	unsigned char vel;
	short pad;
} BUBBLE_STRUCT;
#endif