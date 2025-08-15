#ifndef TOMB4_GAME_FOGBULBSTRUCT_H
#define TOMB4_GAME_FOGBULBSTRUCT_H
#include "game/fvector.h"
typedef struct FOGBULB_STRUCT {
	FVECTOR WorldPos;
	FVECTOR pos;
	FVECTOR vec;
	float rad;
	float sqrad;
	float inv_sqrad;
	float dist;
	int density;
	int inRange;
	int timer;
	int active;
	int FXRad;
	int room_number;
	int r;
	int g;
	int b;
} FOGBULB_STRUCT;
#endif // TOMB4_GAME_FOGBULBSTRUCT_H
