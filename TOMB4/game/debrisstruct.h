#ifndef TOMB4_GAME_DEBRISSTRUCT_H
#define TOMB4_GAME_DEBRISSTRUCT_H
typedef struct TEXTURESTRUCT TEXTURESTRUCT;
typedef struct DEBRIS_STRUCT {
	TEXTURESTRUCT* TextInfo;
	long x;
	long y;
	long z;
	short XYZOffsets1[3];
	short Dir;
	short XYZOffsets2[3];
	short Speed;
	short XYZOffsets3[3];
	short Yvel;
	short Gravity;
	short RoomNumber;
	unsigned char On;
	unsigned char XRot;
	unsigned char YRot;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char Pad[2];
	long color1;
	long color2;
	long color3;
	long ambient;
	long flags;
	unsigned char nBounces;
} DEBRIS_STRUCT;
#endif// TOMB4_GAME_DEBRISSTRUCT_H
