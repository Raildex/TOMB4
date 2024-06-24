#ifndef GUNSHELL_STRUCT_INCLUDED
#define GUNSHELL_STRUCT_INCLUDED
#include "phd3dpos.h"
struct GUNSHELL_STRUCT {
	PHD_3DPOS pos;
	short fallspeed;
	short room_number;
	short speed;
	short counter;
	short DirXrot;
	short object_number;
};
#endif