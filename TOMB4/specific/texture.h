#pragma once
#include "specific/haltexture.h"
typedef struct IDirect3DTexture2 IDirect3DTexture2;
typedef struct IDirectDrawSurface4 IDirectDrawSurface4;
typedef struct TEXTURE {
	HAL_TEXTURE hal;
	unsigned short width;
	unsigned short height;
	unsigned char tpage;
	unsigned char bump;
	long bumptpage;
} TEXTURE;

typedef enum TEXTURE_FORMAT {
	b8g8r8a8,
	r5g5b5a1,
	r4g4b4a4,
	r8g8b8, // used by logo?
} TEXTURE_FORMAT;

typedef void(__cdecl* rgbfunc)(unsigned char*, unsigned char*, unsigned char*);
char CreateTexturePage(long w, long h,TEXTURE_FORMAT tfmt,TEXTURE_FORMAT sfmt, long MipMapCount, void* pSrc, rgbfunc RGBM, HAL_TEXTURE* dst);
long CalcMipMapCount(long w, long h);
void FreeTextures();
