#ifndef WRAITH_STRUCT_INCLUDED
#define WRAITH_STRUCT_INCLUDED
struct WRAITH_STRUCT
{
	PHD_VECTOR pos;
	short xv;
	short yv;
	short zv;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char pad[3];
};
#endif