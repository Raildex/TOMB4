#ifndef DXTEXTUREINFO_INCLUDED
#define DXTEXTUREINFO_INCLUDED
struct DXTEXTUREINFO
{
	DDPIXELFORMAT ddpf;
	unsigned long bpp;
	long bPalette;
	long bAlpha;
	unsigned char rbpp;
	unsigned char gbpp;
	unsigned char bbpp;
	unsigned char abpp;
	unsigned char rshift;
	unsigned char gshift;
	unsigned char bshift;
	unsigned char ashift;
};
#endif