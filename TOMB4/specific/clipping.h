#pragma once
#include "../global/vars.h"

void inject_clipping(bool replace);

long ZClipper(long n, D3DTLBUMPVERTEX* in, D3DTLBUMPVERTEX* out);
long visible_zclip(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2);

#define XYUVGClipper	( (long(__cdecl*)(long, D3DTLBUMPVERTEX*)) 0x0046E8D0 )
