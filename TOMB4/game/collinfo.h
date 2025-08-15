#ifndef TOMB4_GAME_COLLINFO_H
#define TOMB4_GAME_COLLINFO_H
#include "game/collisiontypes.h"
#include "game/heighttypes.h"
#include "game/phdvector.h"
typedef struct COLL_INFO {
	int mid_floor;
	int mid_ceiling;
	height_types mid_type;
	int front_floor;
	int front_ceiling;
	height_types front_type;
	int left_floor;
	int left_ceiling;
	height_types left_type;
	int right_floor;
	int right_ceiling;
	height_types right_type;
	int left_floor2;
	int left_ceiling2;
	height_types left_type2;
	int right_floor2;
	int right_ceiling2;
	height_types right_type2;
	int radius;
	int bad_pos;
	int bad_neg;
	int bad_ceiling;
	PHD_VECTOR shift;
	PHD_VECTOR old;
	short old_anim_state;
	short old_anim_number;
	short old_frame_number;
	short facing;
	short quadrant;
	collision_types coll_type;
	short* trigger;
	char tilt_x;
	char tilt_z;
	char hit_by_baddie;
	char hit_static;
	unsigned short slopes_are_walls : 2;
	unsigned short slopes_are_pits : 1;
	unsigned short lava_is_pit : 1;
	unsigned short enable_baddie_push : 1;
	unsigned short enable_spaz : 1;
	unsigned short hit_ceiling : 1;
} COLL_INFO;
#endif// TOMB4_GAME_COLLINFO_H
