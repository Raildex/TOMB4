#ifndef DRIP_STRUCT_INCLUDED
#define DRIP_STRUCT_INCLUDED
struct DRIP_STRUCT
{
	long x;
	long y;
	long z;
	unsigned char On;
	unsigned char R;
	unsigned char G;
	unsigned char B;
	short Yvel;
	unsigned char Gravity;
	unsigned char Life;
	short RoomNumber;
	unsigned char Outside;
	unsigned char Pad;
};
#endif