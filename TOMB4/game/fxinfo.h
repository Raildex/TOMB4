#ifndef FX_INFO_INCLUDED
#define FX_INFO_INCLUDED
#include "game/phd3dpos.h"
struct FX_INFO {
	PHD_3DPOS pos;
	short room_number;
	short object_number;
	short next_fx;
	short next_active;
	short speed;
	short fallspeed;
	short frame_number;
	short counter;
	short shade;
	short flag1;
	short flag2;
};
#endif