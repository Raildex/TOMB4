#ifndef TOMB4_GAME_MESHINFO_H
#define TOMB4_GAME_MESHINFO_H
#include "game/phdvector.h"
#pragma pack(push, 1)
typedef struct MESH_INFO {
	PHD_VECTOR pos;
	short y_rot;
	short shade;
	short Flags;
	short static_number;
} MESH_INFO;
#pragma pack(pop)
#endif// TOMB4_GAME_MESHINFO_H
