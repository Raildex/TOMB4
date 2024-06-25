#pragma once
#include "global/types.h"
struct IDirect3DTexture2;
struct IDirectDrawSurface4;
#pragma pack(push, 1)
struct TEXTURE {
	IDirect3DTexture2* tex;
	IDirectDrawSurface4* surface;
	unsigned long xoff;
	unsigned long yoff;
	unsigned long width;
	unsigned long height;
	long tpage;
	bool bump;
	long bumptpage;
};
#pragma pack(pop)
typedef void(__cdecl* rgbfunc)(unsigned char*, unsigned char*, unsigned char*);
IDirectDrawSurface4* CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format);
void FreeTextures();

extern TEXTURE* Textures;
extern long nTextures;
