#pragma once
#include "global/types.h"
#include <stdbool.h>
typedef struct IDirect3DTexture2 IDirect3DTexture2;
typedef struct IDirectDrawSurface4 IDirectDrawSurface4;
#pragma pack(push, 1)
typedef struct TEXTURE {
	IDirect3DTexture2* tex;
	IDirectDrawSurface4* surface;
	unsigned long xoff;
	unsigned long yoff;
	unsigned long width;
	unsigned long height;
	long tpage;
	bool bump;
	long bumptpage;
} TEXTURE;
#pragma pack(pop)
typedef void(__cdecl* rgbfunc)(unsigned char*, unsigned char*, unsigned char*);
IDirectDrawSurface4* CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format);
void FreeTextures();

extern TEXTURE* Textures;
extern long nTextures;
