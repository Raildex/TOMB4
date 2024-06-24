#ifndef TOMB4_GAME_SPARKS_H
#define TOMB4_GAME_SPARKS_H
struct SPARKS {
	long x;
	long y;
	long z;
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
	unsigned char TransType;
	unsigned char extras;
	char Dynamic;
	unsigned char FxObj;
	unsigned char RoomNumber;
	unsigned char NodeNumber;
};
#endif // TOMB4_GAME_SPARKS_H
