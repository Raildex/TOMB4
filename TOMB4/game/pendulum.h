#ifndef PENDULUM_INCLUDED
#define PENDULUM_INCLUDED
#include "game/phdvector.h"

typedef struct ROPE_STRUCT ROPE_STRUCT;
typedef struct PENDULUM {
	PHD_VECTOR Position;
	PHD_VECTOR Velocity;
	long node;
	ROPE_STRUCT* Rope;
} PENDULUM;
#endif