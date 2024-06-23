#ifndef AIOBJECT_INCLUDED
#define AIOBJECT_INCLUDED
#pragma pack(push,1)
struct AIOBJECT
{
	short object_number;
	short room_number;
	long x;
	long y;
	long z;
	short trigger_flags;
	short flags;
	short y_rot;
	short box_number;
};
#pragma pack(pop)
#endif