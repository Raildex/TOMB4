#ifndef TOMB4_GAME_ITEMINFO_H
#define TOMB4_GAME_ITEMINFO_H
#include "game/itemlight.h"
#include "game/objects.h"
#include "game/phd3dpos.h"
#pragma pack(push, 1)
typedef struct ITEM_INFO {
	long floor;
	unsigned long touch_bits;
	unsigned long mesh_bits;
	enum object_types object_number;
	short current_anim_state;
	short goal_anim_state;
	short required_anim_state;
	short anim_number;
	short frame_number;
	short room_number;
	short next_item;
	short next_active;
	short speed;
	short fallspeed;
	short hit_points;
	unsigned short box_number;
	short timer;
	short flags;
	short shade;
	short trigger_flags;
	short carried_item;
	short after_death;
	unsigned short fired_weapon;
	short item_flags[4];
	void* data;
	PHD_3DPOS pos;
	ITEM_LIGHT il;
	unsigned long active : 1;
	unsigned long status : 2;
	unsigned long gravity_status : 1;
	unsigned long hit_status : 1;
	unsigned long collidable : 1;
	unsigned long looked_at : 1;
	unsigned long dynamic_light : 1;
	unsigned long poisoned : 1;
	unsigned long ai_bits : 5;
	unsigned long meshswap_meshbits;
} ITEM_INFO;
#pragma pack(pop)
#endif// TOMB4_GAME_ITEMINFO_H
