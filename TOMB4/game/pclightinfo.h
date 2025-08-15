#ifndef TOMB4_GAME_PCLIGHTINFO_H
#define TOMB4_GAME_PCLIGHTINFO_H
typedef struct PCLIGHT_INFO {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	int shadow;
	float Inner;
	float Outer;
	float InnerAngle;
	float OuterAngle;
	float Cutoff;
	float nx;
	float ny;
	float nz;
	int ix;
	int iy;
	int iz;
	int inx;
	int iny;
	int inz;
	unsigned char Type;
	unsigned char Pad;
} PCLIGHT_INFO;
#endif // TOMB4_GAME_PCLIGHTINFO_H
