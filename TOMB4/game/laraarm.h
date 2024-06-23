#ifndef LARA_ARM_INCLUDED
#define LARA_ARM_INCLUDED
#pragma pack(push,1)
struct LARA_ARM
{
	short* frame_base;
	short frame_number;
	short anim_number;
	short lock;
	short y_rot;
	short x_rot;
	short z_rot;
	short flash_gun;
};
#pragma pack(pop)
#endif