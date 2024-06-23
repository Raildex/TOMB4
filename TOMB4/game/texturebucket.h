#ifndef TEXTUREBUCKET_INCLUDED
#define TEXTUREBUCKET_INCLUDED
#include "d3dtlbumpvertex.h"
struct TEXTUREBUCKET
{
	long tpage;
	long nVtx;
	D3DTLBUMPVERTEX vtx[544];
};
#endif