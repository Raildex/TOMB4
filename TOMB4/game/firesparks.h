#ifndef FIRE_SPARKS_INCLUDED
#define FIRE_SPARKS_INCLUDED
struct FIRE_SPARKS
{
	short x;
	short y;
	short z;
	short Xvel;
	short Yvel;
	short Zvel;
	short Gravity;
	short RotAng;
	short Flags;
	unsigned char sSize;
	unsigned char dSize;
	unsigned char Size;
	unsigned char Friction;
	unsigned char Scalar;
	unsigned char Def;
	char RotAdd;
	char MaxYvel;
	unsigned char On;
	unsigned char sR;
	unsigned char sG;
	unsigned char sB;
	unsigned char dR;
	unsigned char dG;
	unsigned char dB;
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char ColFadeSpeed;
	unsigned char FadeToBlack;
	unsigned char sLife;
	unsigned char Life;
};
#endif