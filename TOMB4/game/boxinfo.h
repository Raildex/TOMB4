#ifndef BOX_INFO_INCLUDED
#define BOX_INFO_INCLUDED
#pragma pack(push,1)
struct BOX_INFO
{
	unsigned char left;
	unsigned char right;
	unsigned char top;
	unsigned char bottom;
	short height;
	short overlap_index;
};
#pragma pack(pop)
#endif