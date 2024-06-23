#ifndef FOGBULB_STRUCT_INCLUDED
#define FOGBULB_STRUCT_INCLUDED
#include "fvector.h"
struct FOGBULB_STRUCT
{
	FVECTOR WorldPos;
	FVECTOR pos;
	FVECTOR vec;
	float rad;
	float sqrad;
	float inv_sqrad;
	float dist;
	long density;
	long inRange;
	long timer;
	long active;
	long FXRad;
	long room_number;
	long r;
	long g;
	long b;
};
#endif