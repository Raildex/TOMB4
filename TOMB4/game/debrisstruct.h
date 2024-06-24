#ifndef TOMB4_GAME_DEBRISSTRUCT_H
#define TOMB4_GAME_DEBRISSTRUCT_H
struct DEBRIS_STRUCT {
	void* TextInfo;
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
};
#endif// TOMB4_GAME_DEBRISSTRUCT_H
