#ifndef OBJECT_INFO_INCLUDED
#define OBJECT_INFO_INCLUDED
#include "game/heighttypes.h"
typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;
typedef struct OBJECT_INFO {
	short nmeshes;
	short mesh_index;
	long bone_index;
	short* frame_base;
	void (*initialise)(short item_number);
	void (*control)(short item_number);
	void (*floor)(ITEM_INFO* item, long x, long y, long z, long* height, height_types* height_type, long* tiltxoff, long* tiltzoff, long* OnObject);
	void (*ceiling)(ITEM_INFO* item, long x, long y, long z, long* height);
	void (*draw_routine)(ITEM_INFO* item);
	void (*collision)(short item_num, ITEM_INFO* laraitem, COLL_INFO* coll);
	short object_mip;
	short anim_index;
	short hit_points;
	short pivot_length;
	short radius;
	short shadow_size;
	unsigned short bite_offset;
	unsigned short loaded : 1;
	unsigned short intelligent : 1;
	unsigned short non_lot : 1;
	unsigned short save_position : 1;
	unsigned short save_hitpoints : 1;
	unsigned short save_flags : 1;
	unsigned short save_anim : 1;
	unsigned short semi_transparent : 1;
	unsigned short water_creature : 1;
	unsigned short using_drawanimating_item : 1;
	unsigned short HitEffect : 2;
	unsigned short undead : 1;
	unsigned short save_mesh : 1;
	void (*draw_routine_extra)(ITEM_INFO* item);
	unsigned long explodable_meshbits;
	unsigned long padfuck;
} OBJECT_INFO;
#endif