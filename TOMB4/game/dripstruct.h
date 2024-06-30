#ifndef TOMB4_GAME_DRIPSTRUCT_H
#define TOMB4_GAME_DRIPSTRUCT_H
#include "game/phdvector.h"
typedef struct DRIP_STRUCT {
	PHD_VECTOR pos;
	unsigned char On;
	unsigned char R;
	unsigned char G;
	unsigned char B;
	short Yvel;
	unsigned char Gravity;
	unsigned char Life;
	short RoomNumber;
	unsigned char Outside;
	unsigned char Pad;
} DRIP_STRUCT;
#endif // TOMB4_GAME_DRIPSTRUCT_H
