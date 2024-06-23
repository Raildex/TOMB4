#ifndef PHDSPRITESTRUCT_INCLUDED
#define PHDSPRITESTRUCT_INCLUDED
#pragma pack(push,1)
struct PHDSPRITESTRUCT
{
	unsigned short tpage;
	unsigned short offset;
	unsigned short width;
	unsigned short height;
	short x1;
	short y1;
	short x2;
	short y2;
};
#pragma pack(pop)
#endif