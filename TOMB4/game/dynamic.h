#ifndef DYNAMIC_INCLUDED
#define DYNAMIC_INCLUDED
#include "game/phdvector.h"
typedef struct DYNAMIC {
	PHD_VECTOR pos;
	unsigned char on;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned short falloff;
	unsigned char used;
	unsigned char pad1[1];
} DYNAMIC;
#endif