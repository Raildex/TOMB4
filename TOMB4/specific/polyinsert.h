#pragma once
#include <d3d.h>
struct D3DTLBUMPVERTEX;
struct FOGBULB_STRUCT;
struct TEXTURESTRUCT;
struct FVECTOR;
struct SORTLIST;
struct _D3DTLVERTEX;
void HWR_DrawSortList(D3DTLBUMPVERTEX* info, short num_verts, short texture, short type);
void DrawSortList();
void CreateFogPos(FOGBULB_STRUCT* FogBulb);
void ControlFXBulb(FOGBULB_STRUCT* FogBulb);
void CreateFXBulbs();
void ClearFXFogBulbs();
void TriggerFXFogBulb(long x, long y, long z, long FXRad, long density, long r, long g, long b, long room_number);
long IsVolumetric();
int DistCompare(const void* a, const void* b);
void InitialiseFogBulbs();
void OmniEffect(_D3DTLVERTEX* v);
void OmniFog(_D3DTLVERTEX* v);
void AddTriClippedSorted(_D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
void AddQuadClippedSorted(_D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
void AddLineClippedSorted(_D3DTLVERTEX* v0, _D3DTLVERTEX* v1, short drawtype);
void InitialiseSortList();
void DoSort(long left, long right, SORTLIST** list);
void SortPolyList(long count, SORTLIST** list);
void mD3DTransform(FVECTOR* vec, _D3DMATRIX* mx);
void AddClippedPoly(D3DTLBUMPVERTEX* dest, long nPoints, D3DTLBUMPVERTEX* v, TEXTURESTRUCT* pTex);
void AddTriClippedZBuffer(_D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
void AddQuadClippedZBuffer(_D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
void SubdivideEdge(_D3DTLVERTEX* v0, _D3DTLVERTEX* v1, _D3DTLVERTEX* v, short* c, float tu1, float tv1, float tu2, float tv2, float* tu, float* tv);
void SubdivideQuad(_D3DTLVERTEX* v0, _D3DTLVERTEX* v1, _D3DTLVERTEX* v2, _D3DTLVERTEX* v3, TEXTURESTRUCT* tex, long double_sided, long steps, short* c);
void SubdivideTri(_D3DTLVERTEX* v0, _D3DTLVERTEX* v1, _D3DTLVERTEX* v2, TEXTURESTRUCT* tex, long double_sided, long steps, short* c);
void AddTriSubdivide(_D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
void AddQuadSubdivide(_D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
void CalcColorSplit(D3DCOLOR s, D3DCOLOR* r);

extern D3DTLBUMPVERTEX XYUVClipperBuffer[];
extern D3DTLBUMPVERTEX zClipperBuffer[];

extern FOGBULB_STRUCT FogBulbs[];
extern long NumLevelFogBulbs;

extern long nPolys;
extern long nClippedPolys;
extern long DrawPrimitiveCnt;

extern SORTLIST* SortList[];
extern long SortCount;
