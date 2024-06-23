#ifndef DXDISPLAYMODE_INCLUDED
#define DXDISPLAYMODE_INCLUDED
struct DXDISPLAYMODE
{
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
};
#endif