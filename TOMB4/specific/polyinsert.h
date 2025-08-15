#ifndef TOMB4_SPECIFIC_POLYINSERT_H
#define TOMB4_SPECIFIC_POLYINSERT_H
#include <d3d.h>
typedef struct D3DTLBUMPVERTEX D3DTLBUMPVERTEX;
typedef struct FOGBULB_STRUCT FOGBULB_STRUCT;
typedef struct TEXTURESTRUCT TEXTURESTRUCT;
typedef struct FVECTOR FVECTOR;
typedef struct SORTLIST SORTLIST;
void HWR_DrawSortList(D3DTLBUMPVERTEX* info, short num_verts, short texture, short type);
void DrawSortList();
void CreateFogPos(FOGBULB_STRUCT* FogBulb);
void ControlFXBulb(FOGBULB_STRUCT* FogBulb);
void CreateFXBulbs();
void ClearFXFogBulbs();
void TriggerFXFogBulb(int x, int y, int z, int FXRad, int density, int r, int g, int b, int room_number);
int IsVolumetric();
int DistCompare(const void* a, const void* b);
void InitialiseFogBulbs();
void OmniEffect(D3DTLVERTEX* v);
void OmniFog(D3DTLVERTEX* v);
void AddTriClippedSorted(D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, int double_sided);
void AddQuadClippedSorted(D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, int double_sided);
void AddLineClippedSorted(D3DTLVERTEX* v0, D3DTLVERTEX* v1, short drawtype);
void InitialiseSortList();
void DoSort(int left, int right, SORTLIST** list);
void SortPolyList(int count, SORTLIST** list);
void mD3DTransform(FVECTOR* vec, D3DMATRIX* mx);
void AddClippedPoly(D3DTLBUMPVERTEX* dest, int nPoints, D3DTLBUMPVERTEX* v, TEXTURESTRUCT* pTex);
void AddTriClippedZBuffer(D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, int double_sided);
void AddQuadClippedZBuffer(D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, int double_sided);
void SubdivideEdge(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v, short* c, float tu1, float tv1, float tu2, float tv2, float* tu, float* tv);
void SubdivideQuad(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2, D3DTLVERTEX* v3, TEXTURESTRUCT* tex, int double_sided, int steps, short* c);
void SubdivideTri(D3DTLVERTEX* v0, D3DTLVERTEX* v1, D3DTLVERTEX* v2, TEXTURESTRUCT* tex, int double_sided, int steps, short* c);
void AddTriSubdivide(D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, int double_sided);
void AddQuadSubdivide(D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, int double_sided);
void CalcColorSplit(D3DCOLOR s, D3DCOLOR* r);

extern D3DTLBUMPVERTEX XYUVClipperBuffer[];
extern D3DTLBUMPVERTEX zClipperBuffer[];

extern FOGBULB_STRUCT FogBulbs[];
extern int NumLevelFogBulbs;

extern int nPolys;
extern int nClippedPolys;
extern int DrawPrimitiveCnt;

extern SORTLIST* SortList[];
extern int SortCount;


#endif// TOMB4_SPECIFIC_POLYINSERT_H
