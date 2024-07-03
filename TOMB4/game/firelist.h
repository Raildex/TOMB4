#ifndef TOMB4_GAME_FIRELIST_H
#define TOMB4_GAME_FIRELIST_H
#include "game/phdvector.h"
typedef struct FIRE_LIST {
	PHD_VECTOR pos;
	char on;
	char size;
	short room_number;
} FIRE_LIST;
#endif // TOMB4_GAME_FIRELIST_H
