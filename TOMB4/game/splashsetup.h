#ifndef TOMB4_GAME_SPLASHSETUP_H
#define TOMB4_GAME_SPLASHSETUP_H
#include "game/phdvector.h"
typedef struct SPLASH_SETUP {
	PHD_VECTOR pos;
	short InnerRad;
	short InnerSize;
	short InnerRadVel;
	short InnerYVel;
	short pad1;
	short MiddleRad;
	short MiddleSize;
	short MiddleRadVel;
	short MiddleYVel;
	short pad2;
	short OuterRad;
	short OuterSize;
	short OuterRadVel;
	short pad3;
} SPLASH_SETUP;
#endif // TOMB4_GAME_SPLASHSETUP_H
