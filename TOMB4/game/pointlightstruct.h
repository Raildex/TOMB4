#ifndef TOMB4_GAME_POINTLIGHTSTRUCT_H
#define TOMB4_GAME_POINTLIGHTSTRUCT_H
#include "game/fvector.h"
typedef struct POINTLIGHT_STRUCT {
	FVECTOR vec;
	float r;
	float g;
	float b;
	float rad;
} POINTLIGHT_STRUCT;
#endif // TOMB4_GAME_POINTLIGHTSTRUCT_H
