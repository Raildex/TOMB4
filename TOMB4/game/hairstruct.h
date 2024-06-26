#ifndef HAIR_STRUCT_INCLUDED
#define HAIR_STRUCT_INCLUDED
#include "game/phd3dpos.h"
#include "game/phdvector.h"
typedef struct HAIR_STRUCT {
	PHD_3DPOS pos;
	PHD_VECTOR vel;
} HAIR_STRUCT;
#endif