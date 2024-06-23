#pragma once
#include "types.h"

struct TEXTURE;
struct IDirectDrawSurface4;
typedef void(__cdecl* rgbfunc)(unsigned char*, unsigned char*, unsigned char*);
IDirectDrawSurface4* CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format);
void FreeTextures();

extern TEXTURE* Textures;
extern long nTextures;
