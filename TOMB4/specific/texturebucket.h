#ifndef TOMB4_GAME_TEXTUREBUCKET_H
#define TOMB4_GAME_TEXTUREBUCKET_H
#include "specific/d3dtlbumpvertex.h"
typedef struct TEXTUREBUCKET {
	long tpage;
	long nVtx;
	D3DTLBUMPVERTEX vtx[544];
} TEXTUREBUCKET;
#endif// TOMB4_GAME_TEXTUREBUCKET_H
