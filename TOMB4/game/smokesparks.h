#ifndef TOMB4_GAME_SMOKESPARKS_H
#define TOMB4_GAME_SMOKESPARKS_H
#include "game/phdvector.h"
typedef struct SMOKE_SPARKS {
	PHD_VECTOR pos;
	short Xvel;
	short Yvel;
	short Zvel;
	short Gravity;
	short RotAng;
	short Flags;
	unsigned char sSize;
	unsigned char dSize;
	unsigned char Size;
	unsigned char Friction;
	unsigned char Scalar;
	unsigned char Def;
	char RotAdd;
	char MaxYvel;
	unsigned char On;
	unsigned char sShade;
	unsigned char dShade;
	unsigned char Shade;
	unsigned char ColFadeSpeed;
	unsigned char FadeToBlack;
	char sLife;
	char Life;
	unsigned char TransType;
	unsigned char FxObj;
	unsigned char NodeNumber;
	unsigned char mirror;
} SMOKE_SPARKS;
#endif // TOMB4_GAME_SMOKESPARKS_H
