
#include "game/lot.h"
#include "game/box.h"
#include "game/boxnode.h"
#include "game/camera.h"
#include "game/control.h"
#include "game/creatureinfo.h"
#include "game/floorinfo.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/itemstatus.h"
#include "game/lara.h"
#include "game/larainfo.h"
#include "game/levelinfo.h"
#include "game/lotinfo.h"
#include "game/objects.h"
#include "game/phd3dpos.h"
#include "game/roominfo.h"
#include "game/zonetype.h"
#include "global/types.h"
#include "specific/function_stubs.h"

CREATURE_INFO* baddie_slots;

static long slots_used = 0;

void InitialiseLOTarray(long allocmem) {
	CREATURE_INFO* creature;

	if(allocmem) {
		baddie_slots = (CREATURE_INFO*)Allocate(currentLevel, sizeof(CREATURE_INFO), 5);
	}

	for(int i = 0; i < 5; i++) {
		creature = &baddie_slots[i];
		creature->item_num = NO_ITEM;

		if(allocmem) {
			creature->LOT.node = (BOX_NODE*)Allocate(currentLevel, sizeof(BOX_NODE), GetNumBoxes(currentLevel));
		}
	}

	slots_used = 0;
}

void DisableBaddieAI(short item_number) {
	ITEM_INFO* item;
	CREATURE_INFO* creature;

	item = GetItem(currentLevel, item_number);
	creature = (CREATURE_INFO*)item->data;
	item->data = NULL;

	if(creature) {
		creature->item_num = NO_ITEM;
		slots_used--;
	}
}

void ClearLOT(LOT_INFO* lot) {
	BOX_NODE* node;

	lot->tail = NO_BOX;
	lot->head = NO_BOX;
	lot->search_number = 0;
	lot->target_box = NO_BOX;
	lot->required_box = NO_BOX;
	node = lot->node;

	for(int i = 0; i < GetNumBoxes(currentLevel); i++) {
		node->next_expansion = NO_BOX;
		node->exit_box = NO_BOX;
		node->search_number = 0;
		node++;
	}
}

void CreateZone(ITEM_INFO* item) {
	CREATURE_INFO* creature;
	ROOM_INFO* r;
	BOX_NODE* node;
	short* zone;
	short* flip;
	short zone_number, flip_number;

	creature = (CREATURE_INFO*)item->data;
	r = GetRoom(currentLevel, item->room_number);
	item->box_number = r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)].box;

	if(creature->LOT.fly) {
		creature->LOT.zone_count = 0;
		node = creature->LOT.node;

		for(int i = 0; i < GetNumBoxes(currentLevel); i++) {
			node->box_number = i;
			node++;
			creature->LOT.zone_count++;
		}
	} else {
		zone = GetZone(currentLevel, creature->LOT.zone, 0);
		flip = GetZone(currentLevel, creature->LOT.zone, 1);
		zone_number = zone[item->box_number];
		flip_number = flip[item->box_number];
		creature->LOT.zone_count = 0;
		node = creature->LOT.node;

		for(int i = 0; i < GetNumBoxes(currentLevel); i++) {
			if(*zone == zone_number || *flip == flip_number) {
				node->box_number = i;
				node++;
				creature->LOT.zone_count++;
			}

			zone++;
			flip++;
		}
	}
}

void InitialiseSlot(short item_number, long slot) {
	ITEM_INFO* item;
	CREATURE_INFO* creature;

	creature = &baddie_slots[slot];
	item = GetItem(currentLevel, item_number);
	item->data = creature;
	creature->item_num = item_number;
	creature->mood = BORED_MOOD;
	creature->joint_rotation[0] = 0;
	creature->joint_rotation[1] = 0;
	creature->joint_rotation[2] = 0;
	creature->joint_rotation[3] = 0;
	creature->alerted = 0;
	creature->head_left = 0;
	creature->head_right = 0;
	creature->reached_goal = 0;
	creature->hurt_by_lara = 0;
	creature->patrol2 = 0;
	creature->jump_ahead = 0;
	creature->monkey_ahead = 0;
	creature->LOT.can_jump = 0;
	creature->LOT.can_monkey = 0;
	creature->LOT.is_jumping = 0;
	creature->LOT.is_monkeying = 0;
	creature->maximum_turn = 182;
	creature->flags = 0;
	creature->enemy = NULL;
	creature->LOT.step = 256;
	creature->LOT.drop = -512;
	creature->LOT.block_mask = 0x4000;
	creature->LOT.fly = 0;
	creature->LOT.zone = BASIC_ZONE;

	switch(item->object_number) {
	case ENEMY_JEEP:
	case VON_CROY:
	case RAGHEAD:
	case SUPER_RAGHEAD:
		creature->LOT.step = 1024;
		creature->LOT.drop = -1024;
		creature->LOT.can_jump = 1;
		creature->LOT.can_monkey = 1;
		creature->LOT.zone = HUMAN_ZONE;
		break;

	case GUIDE:
	case MUMMY:
	case SPHINX:
	case HORSEMAN:
	case SCORPION:
	case TROOPS:
	case KNIGHTS_TEMPLAR:
	case MUTANT:
	case WILD_BOAR:
	case DEMIGOD1:
	case DEMIGOD2:
	case DEMIGOD3:
	case FUCKED_UP_DOG:
		creature->LOT.step = 256;
		creature->LOT.drop = -512;
		creature->LOT.zone = BASIC_ZONE;
		break;

	case SKELETON:
	case SETHA:
		creature->LOT.step = 256;
		creature->LOT.drop = -512;
		creature->LOT.can_jump = 1;
		creature->LOT.zone = SKELLY_ZONE;
		break;

	case CROCODILE:
		creature->LOT.step = 20480;
		creature->LOT.drop = -20480;
		creature->LOT.fly = 32;
		creature->LOT.zone = CROC_ZONE;
		break;

	case BAT:
		creature->LOT.step = 20480;
		creature->LOT.drop = -20480;
		creature->LOT.fly = 16;
		creature->LOT.zone = FLYER_ZONE;
		break;

	case HARPY:
	case BIG_BEETLE:
	case HAMMERHEAD:
		creature->LOT.step = 20480;
		creature->LOT.drop = -20480;
		creature->LOT.fly = 32;
		creature->LOT.zone = FLYER_ZONE;
		break;
	}

	ClearLOT(&creature->LOT);

	if(item_number != lara.item_number) {
		CreateZone(item);
	}

	slots_used++;
}

long EnableBaddieAI(short item_number, long Always) {
	ITEM_INFO* item;
	CREATURE_INFO* creature;
	long x, y, z, slot, worstslot, dist, worstdist;

	item = GetItem(currentLevel, item_number);

	if(item->data) {
		return 1;
	}

	if(slots_used < 5) {
		for(slot = 0; slot < 5; slot++) {
			creature = &baddie_slots[slot];

			if(creature->item_num == NO_ITEM) {
				InitialiseSlot(item_number, slot);
				return 1;
			}
		}
	}

	if(Always) {
		worstdist = 0;
	} else {
		x = (item->pos.x_pos - camera.pos.pos.x) >> 8;
		y = (item->pos.y_pos - camera.pos.pos.y) >> 8;
		z = (item->pos.z_pos - camera.pos.pos.z) >> 8;
		worstdist = SQUARE(x) + SQUARE(y) + SQUARE(z);
	}

	worstslot = -1;

	for(slot = 0; slot < 5; slot++) {
		creature = &baddie_slots[slot];
		item = GetItem(currentLevel, creature->item_num);
		x = (item->pos.x_pos - camera.pos.pos.x) >> 8;
		y = (item->pos.y_pos - camera.pos.pos.y) >> 8;
		z = (item->pos.z_pos - camera.pos.pos.z) >> 8;
		dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

		if(dist > worstdist) {
			worstslot = slot;
			worstdist = dist;
		}
	}

	if(worstslot >= 0) {
		GetItem(currentLevel, baddie_slots[worstslot].item_num)->status = ITEM_INVISIBLE;
		DisableBaddieAI(baddie_slots[worstslot].item_num);
		InitialiseSlot(item_number, worstslot);
		return 1;
	}

	return 0;
}
