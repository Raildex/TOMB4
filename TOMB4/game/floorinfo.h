#ifndef FLOOR_INFO_INCLUDED
#define FLOOR_INFO_INCLUDED
#pragma pack(push,1)
struct  FLOOR_INFO
{
	unsigned short index;
	unsigned short fx : 4;
	unsigned short box : 11;
	unsigned short stopper : 1;
	unsigned char pit_room;
	char floor;
	unsigned char sky_room;
	char ceiling;
};
#pragma pack(pop)
#endif