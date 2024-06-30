#ifndef TOMB4_GAME_ITEMLIGHT_H
#define TOMB4_GAME_ITEMLIGHT_H
#include "game/pclight.h"
typedef struct ITEM_LIGHT {
	long r;
	long g;
	long b;
	long ambient;
	long rs;
	long gs;
	long bs;
	long fcnt;
	PCLIGHT CurrentLights[21];
	PCLIGHT PrevLights[21];
	long nCurrentLights;
	long nPrevLights;
	long room_number;
	long RoomChange;
	PHD_VECTOR item_pos;
	void* pCurrentLights;
	void* pPrevLights;
} ITEM_LIGHT;
#endif// TOMB4_GAME_ITEMLIGHT_H
