#ifndef PENDULUM_INCLUDED
#define PENDULUM_INCLUDED
#include "phdvector.h"

struct ROPE_STRUCT;
struct PENDULUM
{
	PHD_VECTOR Position;
	PHD_VECTOR Velocity;
	long node;
	ROPE_STRUCT* Rope;
};
#endif