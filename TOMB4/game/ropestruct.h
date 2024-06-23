#ifndef ROPE_STRUCT_INCLUDED
#define ROPE_STRUCT_INCLUDED
#include "phdvector.h"
struct ROPE_STRUCT
{
	PHD_VECTOR Segment[24];
	PHD_VECTOR Velocity[24];
	PHD_VECTOR NormalisedSegment[24];
	PHD_VECTOR MeshSegment[24];
	PHD_VECTOR Position;
	long Coords[24][3];
	long SegmentLength;
	long Active;
};
#endif