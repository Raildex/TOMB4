#ifndef TOMB4_GAME_TEXTUREBUCKET_H
#define TOMB4_GAME_TEXTUREBUCKET_H
#include "specific/windows/d3dtlbumpvertex.h"
typedef struct TEXTUREBUCKET {
	long tpage;
	long nVtx;
	D3DTLBUMPVERTEX vtx[2048];
} TEXTUREBUCKET;
#endif// TOMB4_GAME_TEXTUREBUCKET_H
