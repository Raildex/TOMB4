#ifndef TOMB4_GAME_SPLASHSTRUCT_H
#define TOMB4_GAME_SPLASHSTRUCT_H
#include "game/phdvector.h"
typedef struct SPLASH_STRUCT {
	PHD_VECTOR pos;
	short InnerRad;
	short InnerSize;
	short InnerRadVel;
	short InnerYVel;
	short InnerY;
	short MiddleRad;
	short MiddleSize;
	short MiddleRadVel;
	short MiddleYVel;
	short MiddleY;
	short OuterRad;
	short OuterSize;
	short OuterRadVel;
	char flags;
	unsigned char life;
} SPLASH_STRUCT;
#endif// TOMB4_GAME_SPLASHSTRUCT_H
