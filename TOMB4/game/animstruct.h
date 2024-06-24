#ifndef ANIM_STRUCT_INCLUDED
#define ANIM_STRUCT_INCLUDED
#pragma pack(push, 1)
struct ANIM_STRUCT {
	short* frame_ptr;
	short interpolation;
	short current_anim_state;
	long velocity;
	long acceleration;
	long Xvelocity;
	long Xacceleration;
	short frame_base;
	short frame_end;
	short jump_anim_num;
	short jump_frame_num;
	short number_changes;
	short change_index;
	short number_commands;
	short command_index;
};
#pragma pack(pop)
#endif