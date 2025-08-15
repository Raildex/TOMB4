#ifndef TOMB4_GAME_DEBRISSTRUCT_H
#define TOMB4_GAME_DEBRISSTRUCT_H
typedef struct TEXTURESTRUCT TEXTURESTRUCT;
typedef struct DEBRIS_STRUCT {
	TEXTURESTRUCT* TextInfo;
	int x;
	int y;
	int z;
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
	int color1;
	int color2;
	int color3;
	int ambient;
	int flags;
	unsigned char nBounces;
} DEBRIS_STRUCT;
#endif// TOMB4_GAME_DEBRISSTRUCT_H
