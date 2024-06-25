#pragma once
#include "global/math_tbls.h"

#pragma pack(push, 1)

/*math*/
#define SQUARE(x) ((x) * (x))
#define TRIGMULT2(a, b) (((a) * (b)) >> W2V_SHIFT)
#define TRIGMULT3(a, b, c) (TRIGMULT2((TRIGMULT2(a, b)), c))
#define FTRIGMULT2(a, b) ((a) * (b))
#define FTRIGMULT3(a, b, c) (FTRIGMULT2((FTRIGMULT2(a, b)), c))

/*color*/
#define RGBONLY(r, g, b) ((b) | (((g) | ((r) << 8)) << 8))
#define RGBA(r, g, b, a) (RGBONLY(r, g, b) | ((a) << 24))
#define CLRA(clr) ((clr >> 24) & 0xFF) // shift r, g, and b out of the way and 0xFF
#define CLRR(clr) ((clr >> 16) & 0xFF) // shift g and b out of the way and 0xFF
#define CLRG(clr) ((clr >> 8) & 0xFF) // shift b out of the way and 0xFF
#define CLRB(clr) ((clr) & 0xFF) // and 0xFF

/*misc*/
#define SCRIPT_TEXT(num) (&gfStringWad[gfStringOffset[num]])
#define SetCutPlayed(num) (CutSceneTriggered |= 1 << (num))
#define SetCutNotPlayed(num) (CutSceneTriggered &= ~(1 << (num)))
#define CheckCutPlayed(num) (CutSceneTriggered & (1 << (num)))

#define NO_HEIGHT -32512
#define NO_ITEM -1
#define FVF (D3DFVF_TEX2 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW)
#define W2V_SHIFT 14
#define MAX_SAMPLES 370
#define MAX_DYNAMICS 64
#define MALLOC_SIZE 1024 * 1024 * 128 // 128MB

/********************DX defs********************/
#define TEXGUID IID_IDirect3DTexture2
#define DDGUID IID_IDirectDraw4
#define D3DGUID IID_IDirect3D3
#define DIGUID IID_IDirectInput8
#define DIDGUID IID_IDirectInputDevice8
#define DSNGUID IID_IDirectSoundNotify
/***********************************************/


#pragma pack(pop)