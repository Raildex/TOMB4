#ifndef TOMB4_GAME_DXTEXTUREINFO_H
#define TOMB4_GAME_DXTEXTUREINFO_H
#include <ddraw.h>
typedef struct DXTEXTUREINFO {
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
} DXTEXTUREINFO;
#endif // TOMB4_GAME_DXTEXTUREINFO_H
