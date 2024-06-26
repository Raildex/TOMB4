#pragma once

typedef struct D3DTLBUMPVERTEX D3DTLBUMPVERTEX;
typedef struct _D3DTLVERTEX _D3DTLVERTEX;

long ZClipper(long n, D3DTLBUMPVERTEX* in, D3DTLBUMPVERTEX* out);
long visible_zclip(_D3DTLVERTEX* v0, _D3DTLVERTEX* v1, _D3DTLVERTEX* v2);
long XYUVGClipper(long n, D3DTLBUMPVERTEX* in);
