
#include "game/deathsld.h"
#include "game/animstruct.h"
#include "game/collide.h"
#include "game/control.h"
#include "game/gamevector.h"
#include "game/heighttypes.h"
#include "game/inputbuttons.h"
#include "game/itemflags.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/itemstatus.h"
#include "game/lara.h"
#include "game/lara_states.h"
#include "game/laragunstatus.h"
#include "game/larainfo.h"
#include "game/laramisc.h"
#include "game/levelinfo.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/phdvector.h"
#include "game/sound.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/input.h"


static short DeathSlideBounds[12] = { -256, 256, -100, 100, 256, 512, 0, 0, -4550, 4550, 0, 0 };
static PHD_VECTOR DeathSlidePosition = { 0, 0, 371 };

void InitialiseDeathSlide(short item_number) {
	ITEM_INFO* item;
	GAME_VECTOR* old;

	item = GetItem(currentLevel, item_number);
	old = (GAME_VECTOR*)Allocate(currentLevel, sizeof(GAME_VECTOR), 1);
	item->data = old;
	old->pos.x = item->pos.pos.x;
	old->pos.y = item->pos.pos.y;
	old->pos.z = item->pos.pos.z;
	old->room_number = item->room_number;
}

void DeathSlideCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll) {
	ITEM_INFO* item;

	if(S_IsActionDown(inputImpl, IN_ACTION) && !l->gravity_status && lara.gun_status == LG_NO_ARMS && l->current_anim_state == AS_STOP) {
		item = GetItem(currentLevel, item_number);

		if(item->status == ITEM_INACTIVE) {
			if(TestLaraPosition(DeathSlideBounds, item, l)) {
				AlignLaraPosition(&DeathSlidePosition, item, l);
				lara.gun_status = LG_HANDS_BUSY;
				l->goal_anim_state = AS_DEATHSLIDE;

				do {
					AnimateLara(l);
				} while(l->current_anim_state != AS_NULL);

				if(!item->active) {
					AddActiveItem(item_number);
				}

				item->status = ITEM_ACTIVE;
				item->flags |= IFL_INVISIBLE;
			}
		}
	}
}

void ControlDeathSlide(short item_number) {
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	GAME_VECTOR* old;
	height_types ht;
	long tiltxoff, tiltzoff, OnObject;
	long x, y, z, h, c;
	short room_number;

	item = GetItem(currentLevel, item_number);

	if(item->status != ITEM_ACTIVE) {
		return;
	}

	if(item->flags & IFL_INVISIBLE) {
		if(item->current_anim_state == 1) {
			AnimateItem(item);
			return;
		}

		AnimateItem(item);

		if(item->fallspeed < 100) {
			item->fallspeed += 5;
		}

		item->pos.pos.x += item->fallspeed * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
		item->pos.pos.y += item->fallspeed >> 2;
		item->pos.pos.z += item->fallspeed * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
		room_number = item->room_number;
		GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);

		if(room_number != item->room_number) {
			ItemNewRoom(item_number, room_number);
		}

		if(lara_item->current_anim_state == AS_DEATHSLIDE) {
			lara_item->pos.pos.x = item->pos.pos.x;
			lara_item->pos.pos.y = item->pos.pos.y;
			lara_item->pos.pos.z = item->pos.pos.z;
		}

		x = item->pos.pos.x + (phd_sin(item->pos.y_rot) >> 4);
		y = item->pos.pos.y + 64;
		z = item->pos.pos.z + (phd_cos(item->pos.y_rot) >> 4);
		floor = GetFloor(x, y, z, &room_number);
		h = GetHeight(floor, x, y, z, &ht, &tiltxoff, &tiltzoff, &OnObject);
		c = GetCeiling(floor, x, y, z);

		if(h <= y + 256 || c >= y - 256) {
			if(lara_item->current_anim_state == AS_DEATHSLIDE) {
				lara_item->goal_anim_state = 3;
				AnimateLara(lara_item);
				lara_item->gravity_status = 1;
				lara_item->speed = item->fallspeed;
				lara_item->fallspeed = item->fallspeed >> 2;
			}

			SoundEffect(SFX_VONCROY_KNIFE_SWISH, (PHD_VECTOR*)&item->pos, SFX_DEFAULT);
			RemoveActiveItem(item_number);
			item->status = ITEM_INACTIVE;
			item->flags -= IFL_INVISIBLE;
		} else {
			SoundEffect(SFX_TRAIN_DOOR_CLOSE, (PHD_VECTOR*)&item->pos, SFX_DEFAULT);
		}
	} else {
		old = (GAME_VECTOR*)item->data;
		item->pos.pos.x = old->pos.x;
		item->pos.pos.y = old->pos.y;
		item->pos.pos.z = old->pos.z;

		if(old->room_number != item->room_number) {
			ItemNewRoom(item_number, old->room_number);
		}

		item->status = ITEM_INACTIVE;
		item->current_anim_state = 1;
		item->goal_anim_state = 1;
		item->anim_number = GetObjectInfo(currentLevel, item->object_number)->anim_index;
		item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
		RemoveActiveItem(item_number);
	}
}
