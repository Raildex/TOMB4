#ifndef TOMB4_GAME_LIGHTNINGSTRUCT_H
#define TOMB4_GAME_LIGHTNINGSTRUCT_H
#include "game/phdvector.h"
typedef struct LIGHTNING_STRUCT {
	PHD_VECTOR Point[4];
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char Life;
	char Xvel1;
	char Yvel1;
	char Zvel1;
	char Xvel2;
	char Yvel2;
	char Zvel2;
	char Xvel3;
	char Yvel3;
	char Zvel3;
	unsigned char Size;
	unsigned char Flags;
	unsigned char Rand;
	unsigned char Segments;
	unsigned char Pad[3];
} LIGHTNING_STRUCT;
#endif // TOMB4_GAME_LIGHTNINGSTRUCT_H
