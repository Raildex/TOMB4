#ifndef TOMB4_SPECIFIC_FUNCTION_TABLE_H
#define TOMB4_SPECIFIC_FUNCTION_TABLE_H
#include <windows.h>
#include <stdbool.h>
#include <d3dtypes.h>
typedef struct TEXTURESTRUCT TEXTURESTRUCT;
typedef struct _D3DTLVERTEX _D3DTLVERTEX;

void SetFogColor(long r, long g, long b);
void HWInitialise();
bool _NVisible(_D3DTLVERTEX* v0, _D3DTLVERTEX* v1, _D3DTLVERTEX* v2);
bool _Visible(_D3DTLVERTEX* v0, _D3DTLVERTEX* v1, _D3DTLVERTEX* v2);
void SetCullCW();
void SetCullCCW();
HRESULT HWBeginScene();
HRESULT HWEndScene();
void InitialiseFunctionTable();

extern void (*AddQuadSorted)(_D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddTriSorted)(_D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddQuadZBuffer)(_D3DTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddTriZBuffer)(_D3DTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddLineSorted)(_D3DTLVERTEX* v0, _D3DTLVERTEX* v1, short drawtype);
extern bool (*IsVisible)(_D3DTLVERTEX* v0, _D3DTLVERTEX* v1, _D3DTLVERTEX* v2);
extern HRESULT (*_BeginScene)();
extern HRESULT (*_EndScene)();

extern _D3DTLVERTEX MyVertexBuffer[];
extern long CurrentFog;


#endif// TOMB4_SPECIFIC_FUNCTION_TABLE_H
