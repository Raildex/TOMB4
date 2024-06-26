#ifndef SCARAB_STRUCT_INCLUDED
#define SCARAB_STRUCT_INCLUDED
#include "game/phd3dpos.h"
typedef struct SCARAB_STRUCT {
	PHD_3DPOS pos;
	short room_number;
	short speed;
	short fallspeed;
	unsigned char On;
	unsigned char flags;
} SCARAB_STRUCT;
#endif