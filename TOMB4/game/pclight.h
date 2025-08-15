#ifndef TOMB4_GAME_PCLIGHT_H
#define TOMB4_GAME_PCLIGHT_H
#include "game/phdvector.h"
typedef struct PCLIGHT {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	int shadow;
	float Inner;
	float Outer;
	float InnerAngle;
	float OuterAngle;
	float Cutoff;
	float nx;
	float ny;
	float nz;
	int ix;
	int iy;
	int iz;
	int inx;
	int iny;
	int inz;
	float tr;
	float tg;
	float tb;
	float rs;
	float gs;
	float bs;
	int fcnt;
	unsigned char Type;
	unsigned char Active;
	PHD_VECTOR rlp;
	int Range;
} PCLIGHT;
#endif// TOMB4_GAME_PCLIGHT_H
