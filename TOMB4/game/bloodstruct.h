#ifndef BLOOD_STRUCT_INCLUDED
#define BLOOD_STRUCT_INCLUDED
struct BLOOD_STRUCT {
	long x;
	long y;
	long z;
	short Xvel;
	short Yvel;
	short Zvel;
	short Gravity;
	short RotAng;
	unsigned char sSize;
	unsigned char dSize;
	unsigned char Size;
	unsigned char Friction;
	char RotAdd;
	unsigned char On;
	unsigned char sShade;
	unsigned char dShade;
	unsigned char Shade;
	unsigned char ColFadeSpeed;
	unsigned char FadeToBlack;
	char sLife;
	char Life;
	char Pad;
};
#endif