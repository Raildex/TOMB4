#pragma once

struct D3DTLBUMPVERTEX;
struct _D3DTLVERTEX;

long ZClipper(long n, D3DTLBUMPVERTEX* in, D3DTLBUMPVERTEX* out);
long visible_zclip(_D3DTLVERTEX* v0, _D3DTLVERTEX* v1, _D3DTLVERTEX* v2);
long XYUVGClipper(long n, D3DTLBUMPVERTEX* in);
