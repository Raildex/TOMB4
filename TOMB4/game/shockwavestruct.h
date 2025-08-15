#ifndef SHOCKWAVE_STRUCT_INCLUDED
#define SHOCKWAVE_STRUCT_INCLUDED
typedef struct SHOCKWAVE_STRUCT {
	int x;
	int y;
	int z;
	short InnerRad;
	short OuterRad;
	short XRot;
	short Flags;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char life;
	short Speed;
	short Temp;
} SHOCKWAVE_STRUCT;
#endif
