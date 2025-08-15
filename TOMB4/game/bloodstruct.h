#ifndef BLOOD_STRUCT_INCLUDED
#define BLOOD_STRUCT_INCLUDED
typedef struct BLOOD_STRUCT {
	int x;
	int y;
	int z;
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
	char underwater;
} BLOOD_STRUCT;
#endif
