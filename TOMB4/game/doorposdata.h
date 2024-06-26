#ifndef DOORPOS_DATA_INCLUDED
#define DOORPOS_DATA_INCLUDED
#include "game/floorinfo.h"
typedef struct DOORPOS_DATA {
	FLOOR_INFO* floor;
	FLOOR_INFO data;
	short block;
} DOORPOS_DATA;
#endif