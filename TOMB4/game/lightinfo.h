#ifndef LIGHTINFO_INCLUDED
#define LIGHTINFO_INCLUDED
#pragma pack(push,1)
struct LIGHTINFO
{
	long x;
	long y;
	long z;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char Type;
	short Intensity;
	float Inner;
	float Outer;
	float Length;
	float Cutoff;
	float nx;
	float ny;
	float nz;
};
#pragma pack(pop)
#endif