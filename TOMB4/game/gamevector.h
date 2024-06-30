#ifndef TOMB4_GAME_GAMEVECTOR_H
#define TOMB4_GAME_GAMEVECTOR_H
#include "game/phdvector.h"
typedef struct GAME_VECTOR {
	PHD_VECTOR pos;
	short room_number;
	short box_number;
} GAME_VECTOR;
#endif // TOMB4_GAME_GAMEVECTOR_H
