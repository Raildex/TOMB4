#ifndef TOMB4_GAME_DOORDATA_H
#define TOMB4_GAME_DOORDATA_H
#include "game/doorposdata.h"
typedef struct DOOR_DATA {
	DOORPOS_DATA d1;
	DOORPOS_DATA d1flip;
	DOORPOS_DATA d2;
	DOORPOS_DATA d2flip;
	short Opened;
} DOOR_DATA;
#endif // TOMB4_GAME_DOORDATA_H
