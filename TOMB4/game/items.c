
#include "game/items.h"
#include "game/animstruct.h"
#include "game/control.h"
#include "game/effect2.h"
#include "game/effects.h"
#include "game/floorinfo.h"
#include "game/fxinfo.h"
#include "game/itemflags.h"
#include "game/iteminfo.h"
#include "game/itemstatus.h"
#include "game/lara.h"
#include "game/larainfo.h"
#include "game/levelinfo.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/roominfo.h"
#include "global/types.h"
#include "specific/function_stubs.h"

short next_fx_active;
short next_item_active;

static short next_fx_free;
static short next_item_free;

void InitialiseItemArray(short num, long count) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, count);
	next_item_free = (short)count;
	next_item_active = NO_ITEM;

	for(int i = count + 1; i < num; i++) {
		item->next_item = i;
		item->active = 0;
		item++;
	}

	item->next_item = NO_ITEM;
}

void KillItem(short item_num) {
	ITEM_INFO* item;
	short linknum;

	if(InItemControlLoop) {
		ItemNewRooms[ItemNewRoomNo][0] = item_num | 0x8000;
		ItemNewRoomNo++;
		return;
	}

	DetatchSpark(item_num, 128);
	item = GetItem(currentLevel, item_num);
	item->active = 0;
	void* data = item->data;

	if(next_item_active == item_num) {
		next_item_active = item->next_active;
	} else {
		for(linknum = next_item_active; linknum != NO_ITEM; linknum = GetItem(currentLevel, linknum)->next_active) {
			if(GetItem(currentLevel, linknum)->next_active == item_num) {
				GetItem(currentLevel, linknum)->next_active = item->next_active;
				break;
			}
		}
	}

	if(item->room_number != 255) {
		linknum = GetRoom(currentLevel, item->room_number)->item_number;

		if(linknum == item_num) {
			GetRoom(currentLevel, item->room_number)->item_number = item->next_item;
		} else {
			for(; linknum != NO_ITEM; linknum = GetItem(currentLevel, linknum)->next_item) {
				if(GetItem(currentLevel, linknum)->next_item == item_num) {
					GetItem(currentLevel, linknum)->next_item = item->next_item;
					break;
				}
			}
		}
	}

	if(item_num == lara.target_item) {
		lara.target_item = NO_ITEM;
	}

	if(item_num < GetNumLevelItems(currentLevel)) {
		item->flags |= IFL_CLEARBODY;
	} else {
		item->next_item = next_item_free;
		next_item_free = item_num;
	}
	Deallocate(currentLevel, data);
}

short CreateItem() {
	short item_num;

	item_num = next_item_free;

	if(item_num != NO_ITEM) {
		GetItem(currentLevel, item_num)->flags = 0;
		next_item_free = GetItem(currentLevel, item_num)->next_item;
	}

	return item_num;
}

void InitialiseItem(short item_num) {
	ITEM_INFO* item;
	ROOM_INFO* r;
	FLOOR_INFO* floor;

	item = GetItem(currentLevel, item_num);
	item->anim_number = GetObjectInfo(currentLevel, item->object_number)->anim_index;
	item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
	item->current_anim_state = GetAnim(currentLevel, item->anim_number)->current_anim_state;
	item->goal_anim_state = GetAnim(currentLevel, item->anim_number)->current_anim_state;
	item->required_anim_state = 0;
	item->pos.x_rot = 0;
	item->pos.z_rot = 0;
	item->fallspeed = 0;
	item->speed = 0;
	item->active = 0;
	item->status = ITEM_INACTIVE;
	item->gravity_status = 0;
	item->hit_status = 0;
	item->looked_at = 0;
	item->dynamic_light = 0;
	item->ai_bits = 0;
	item->item_flags[0] = 0;
	item->item_flags[1] = 0;
	item->item_flags[2] = 0;
	item->item_flags[3] = 0;
	item->hit_points = GetObjectInfo(currentLevel, item->object_number)->hit_points;
	item->poisoned = 0;
	item->collidable = 1;
	item->timer = 0;

	if(item->object_number == SIXSHOOTER_ITEM || item->object_number == CROSSBOW_ITEM || item->object_number == SHOTGUN_ITEM) {
		item->mesh_bits = 1;
	} else if(item->object_number == SARCOPHAGUS_CUT) {
		item->mesh_bits = 5;
	} else if(item->object_number == HORUS_STATUE) {
		item->mesh_bits = 1607;
	} else {
		item->mesh_bits = -1;
	}

	item->touch_bits = 0;
	item->after_death = 0;
	item->fired_weapon = 0;
	item->data = NULL;

	if(item->flags & IFL_INVISIBLE) {
		item->status = ITEM_INVISIBLE;
		item->flags -= IFL_INVISIBLE;
	} else if(GetObjectInfo(currentLevel, item->object_number)->intelligent) {
		item->status = ITEM_INVISIBLE;
	}

	if((item->flags & IFL_CODEBITS) == IFL_CODEBITS) {
		item->flags -= IFL_CODEBITS;
		item->flags |= IFL_REVERSE;
		AddActiveItem(item_num);
		item->status = ITEM_ACTIVE;
	}

	r = GetRoom(currentLevel, item->room_number);
	item->next_item = r->item_number;
	r->item_number = item_num;
	floor = &r->floor[((item->pos.pos.z - r->z) >> 10) + r->x_size * ((item->pos.pos.x - r->x) >> 10)];
	item->floor = floor->floor << 8;
	item->box_number = floor->box;

	if(GetObjectInfo(currentLevel, item->object_number)->initialise) {
		GetObjectInfo(currentLevel, item->object_number)->initialise(item_num);
	}

	item->il.fcnt = -1;
	item->il.room_number = -1;
	item->il.nCurrentLights = 0;
	item->il.nPrevLights = 0;
	item->il.ambient = r->ambient;
	item->il.pCurrentLights = item->il.CurrentLights;
	item->il.pPrevLights = item->il.PrevLights;
}

void RemoveActiveItem(short item_num) {
	short linknum;

	if(!GetItem(currentLevel, item_num)->active) {
		return;
	}

	GetItem(currentLevel, item_num)->active = 0;

	if(next_item_active == item_num) {
		next_item_active = GetItem(currentLevel, item_num)->next_active;
	} else {
		for(linknum = next_item_active; linknum != NO_ITEM; linknum = GetItem(currentLevel, linknum)->next_active) {
			if(GetItem(currentLevel, linknum)->next_active == item_num) {
				GetItem(currentLevel, linknum)->next_active = GetItem(currentLevel, linknum)->next_active;
				break;
			}
		}
	}
}

void RemoveDrawnItem(short item_num) {
	ITEM_INFO* item;
	short linknum;

	item = GetItem(currentLevel, item_num);
	linknum = GetRoom(currentLevel, item->room_number)->item_number;

	if(linknum == item_num) {
		GetRoom(currentLevel, item->room_number)->item_number = item->next_item;
	} else {
		for(; linknum != NO_ITEM; linknum = GetItem(currentLevel, linknum)->next_item) {
			if(GetItem(currentLevel, linknum)->next_item == item_num) {
				GetItem(currentLevel, linknum)->next_item = item->next_item;
				break;
			}
		}
	}
}

void AddActiveItem(short item_num) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_num);
	item->flags |= IFL_TRIGGERED;

	if(GetObjectInfo(currentLevel, item->object_number)->control) {
		if(!item->active) {
			item->active = 1;
			item->next_active = next_item_active;
			next_item_active = item_num;
		}
	} else {
		item->status = ITEM_INACTIVE;
	}
}

void ItemNewRoom(short item_num, short room_num) {
	ITEM_INFO* item;
	ROOM_INFO* r;
	short linknum;

	if(InItemControlLoop) {
		ItemNewRooms[ItemNewRoomNo][0] = item_num;
		ItemNewRooms[ItemNewRoomNo][1] = room_num;
		ItemNewRoomNo++;
		return;
	}

	item = GetItem(currentLevel, item_num);

	if(item->room_number != 255) {
		r = GetRoom(currentLevel, item->room_number);
		linknum = r->item_number;

		if(linknum == item_num) {
			r->item_number = item->next_item;
		} else {
			for(; linknum != NO_ITEM; linknum = GetItem(currentLevel, linknum)->next_item) {
				if(GetItem(currentLevel, linknum)->next_item == item_num) {
					GetItem(currentLevel, linknum)->next_item = item->next_item;
					break;
				}
			}
		}
	}

	item->room_number = room_num;
	item->next_item = GetRoom(currentLevel, room_num)->item_number;
	GetRoom(currentLevel, room_num)->item_number = item_num;
}

void InitialiseFXArray() {
	FX_INFO* fx;

	next_fx_active = NO_ITEM;
	next_fx_free = 0;
	fx = GetEffect(currentLevel, 0);

	for(int i = 1; i < 128; i++) {
		fx->next_fx = i;
		fx++;
	}

	fx->next_fx = NO_ITEM;
}

short CreateEffect(short room_num) {
	FX_INFO* fx;
	ROOM_INFO* r;
	short fx_num;

	fx_num = next_fx_free;

	if(fx_num != NO_ITEM) {
		fx = GetEffect(currentLevel, fx_num);
		next_fx_free = fx->next_fx;
		r = GetRoom(currentLevel, room_num);
		fx->room_number = room_num;
		fx->next_fx = r->fx_number;
		r->fx_number = fx_num;
		fx->next_active = next_fx_active;
		next_fx_active = fx_num;
		fx->shade = 0x4210;
	}

	return fx_num;
}

void KillEffect(short fx_num) {
	FX_INFO* fx;
	short linknum;

	if(InItemControlLoop) {
		ItemNewRooms[ItemNewRoomNo][0] = fx_num | 0x8000;
		ItemNewRoomNo++;
		return;
	}

	DetatchSpark(fx_num, 64);
	fx = GetEffect(currentLevel, fx_num);

	if(next_fx_active == fx_num) {
		next_fx_active = fx->next_active;
	} else {
		for(linknum = next_fx_active; linknum != NO_ITEM; linknum = GetEffect(currentLevel, linknum)->next_active) {
			if(GetEffect(currentLevel, linknum)->next_active == fx_num) {
				GetEffect(currentLevel, linknum)->next_active = fx->next_active;
				break;
			}
		}
	}

	linknum = GetRoom(currentLevel, fx->room_number)->fx_number;

	if(linknum == fx_num) {
		GetRoom(currentLevel, fx->room_number)->fx_number = fx->next_fx;
	} else {
		for(; linknum != NO_ITEM; linknum = GetEffect(currentLevel, linknum)->next_fx) {
			if(GetEffect(currentLevel, linknum)->next_fx == fx_num) {
				GetEffect(currentLevel, linknum)->next_fx = fx->next_fx;
				break;
			}
		}
	}

	fx->next_fx = next_fx_free;
	next_fx_free = fx_num;
}

void EffectNewRoom(short fx_num, short room_num) {
	FX_INFO* fx;
	ROOM_INFO* r;
	short linknum;

	if(InItemControlLoop) {
		ItemNewRooms[ItemNewRoomNo][0] = fx_num;
		ItemNewRooms[ItemNewRoomNo][1] = room_num;
		ItemNewRoomNo++;
		return;
	}

	fx = GetEffect(currentLevel, fx_num);
	r = GetRoom(currentLevel, fx->room_number);

	if(r->fx_number == fx_num) {
		r->fx_number = fx->next_fx;
	} else {
		for(linknum = r->fx_number; linknum != NO_ITEM; linknum = GetEffect(currentLevel, linknum)->next_fx) {
			if(GetEffect(currentLevel, linknum)->next_fx == fx_num) {
				GetEffect(currentLevel, linknum)->next_fx = fx->next_fx;
				break;
			}
		}
	}

	fx->room_number = room_num;
	fx->next_fx = GetRoom(currentLevel, room_num)->fx_number;
	GetRoom(currentLevel, room_num)->fx_number = fx_num;
}
