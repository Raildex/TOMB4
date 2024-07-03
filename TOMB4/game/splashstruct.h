#ifndef SPLASH_STRUCT_INCLUDED
#define SPLASH_STRUCT_INCLUDED
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
#endif