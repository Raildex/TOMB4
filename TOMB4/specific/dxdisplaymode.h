#ifndef TOMB4_GAME_DXDISPLAYMODE_H
#define TOMB4_GAME_DXDISPLAYMODE_H
#include <ddraw.h>
typedef struct DXDISPLAYMODE {
	long w;
	long h;
	long bpp;
	long RefreshRate;
	long bPalette;
	DDSURFACEDESC2 ddsd;
	unsigned char rbpp;
	unsigned char gbpp;
	unsigned char bbpp;
	unsigned char rshift;
	unsigned char gshift;
	unsigned char bshift;
} DXDISPLAYMODE;
#endif // TOMB4_GAME_DXDISPLAYMODE_H
