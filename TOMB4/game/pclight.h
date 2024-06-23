#ifndef PCLIGHT_INCLUDED
#define PCLIGHT_INCLUDED
#include "phdvector.h"
struct PCLIGHT
{
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	long shadow;
	float Inner;
	float Outer;
	float InnerAngle;
	float OuterAngle;
	float Cutoff;
	float nx;
	float ny;
	float nz;
	long ix;
	long iy;
	long iz;
	long inx;
	long iny;
	long inz;
	float tr;
	float tg;
	float tb;
	float rs;
	float gs;
	float bs;
	long fcnt;
	unsigned char Type;
	unsigned char Active;
	PHD_VECTOR rlp;
	long Range;
};
#endif