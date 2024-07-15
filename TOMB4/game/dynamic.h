#ifndef TOMB4_GAME_DYNAMIC_H
#define TOMB4_GAME_DYNAMIC_H
#include "game/phdvector.h"
typedef struct DYNAMIC {
	PHD_VECTOR pos;
	unsigned char on;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned short falloff;
} DYNAMIC;
#endif// TOMB4_GAME_DYNAMIC_H
