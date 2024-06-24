#ifndef TOMB4_GAME_PCLIGHTINFO_H
#define TOMB4_GAME_PCLIGHTINFO_H
struct PCLIGHT_INFO {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	long shadow;
	float Inner;
	float Outer;
	float InnerAngle;
	float OuterAngle;
	float Cutoff;
	float nx;
	float ny;
	float nz;
	long ix;
	long iy;
	long iz;
	long inx;
	long iny;
	long inz;
	unsigned char Type;
	unsigned char Pad;
};
#endif // TOMB4_GAME_PCLIGHTINFO_H
