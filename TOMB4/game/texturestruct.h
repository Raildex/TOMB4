#ifndef TEXTURESTRUCT_INCLUDED
#define TEXTURESTRUCT_INCLUDED
#pragma pack(push,1)
struct TEXTURESTRUCT
{
	unsigned short drawtype;
	unsigned short tpage;
	unsigned short flag;
	float u1;
	float v1;
	float u2;
	float v2;
	float u3;
	float v3;
	float u4;
	float v4;
};
#pragma pack(pop)
#endif