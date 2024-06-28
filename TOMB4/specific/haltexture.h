#ifndef TOMB4_SPECIFIC_HALTEXTURE_H
#define TOMB4_SPECIFIC_HALTEXTURE_H
typedef struct IDirectDrawSurface4 IDirectDrawSurface4;
typedef struct IDirect3DTexture2 IDirect3DTexture2;
typedef struct HAL_TEXTURE {
	#if 1
	IDirectDrawSurface4* surface;
	IDirect3DTexture2* dxTex;
	#endif
} HAL_TEXTURE;

#endif// TOMB4_SPECIFIC_HALTEXTURE_H
