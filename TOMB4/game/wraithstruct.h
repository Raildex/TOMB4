#ifndef TOMB4_GAME_WRAITHSTRUCT_H
#define TOMB4_GAME_WRAITHSTRUCT_H
#include "game/phdvector.h"
typedef struct WRAITH_STRUCT {
	PHD_VECTOR pos;
	short xv;
	short yv;
	short zv;
	unsigned char r;
	unsigned char g;
	unsigned char b;
} WRAITH_STRUCT;
#endif // TOMB4_GAME_WRAITHSTRUCT_H
