#ifndef ITEM_LIGHT_INCLUDED
#define ITEM_LIGHT_INCLUDED
#include "pclight.h"
struct ITEM_LIGHT {
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
};
#endif