
#include "game/bat.h"
#include "distances.h"
#include "game/aiinfo.h"
#include "game/animstruct.h"
#include "game/biteinfo.h"
#include "game/box.h"
#include "game/control.h"
#include "game/creatureinfo.h"
#include "game/effects.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/lara.h"
#include "game/levelinfo.h"
#include "game/lot.h"
#include "game/moodtype.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "global/types.h"
#include "specific/function_stubs.h"
#include <stdlib.h>

static BITE_INFO bat_bite = { 0, 16, 45, 4 };

void InitialiseBat(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);
	InitialiseCreature(item_number);
	item->anim_number = GetObjectInfo(currentLevel, BAT)->anim_index + 5;
	item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
	item->goal_anim_state = 6;
	item->current_anim_state = 6;
}

void BatControl(short item_number) {
	ITEM_INFO* item;
	ITEM_INFO* item2;
	CREATURE_INFO* bat;
	CREATURE_INFO* baddie;
	AI_INFO info;
	long max_dist, dist, dx, dz;
	short angle;

	if(!CreatureActive(item_number)) {
		return;
	}

	item = GetItem(currentLevel, item_number);
	bat = (CREATURE_INFO*)item->data;
	angle = 0;

	if(item->hit_points <= 0) {
		if(item->current_anim_state == 3) {
			item->anim_number = GetObjectInfo(currentLevel, BAT)->anim_index + 1;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 2;
			item->goal_anim_state = 2;
		} else {
			if(item->pos.pos.y >= item->floor) {
				item->goal_anim_state = 5;
				item->pos.pos.y = item->floor;
				item->gravity_status = 0;
			} else {
				item->gravity_status = 1;
				item->speed = 0;
				item->anim_number = GetObjectInfo(currentLevel, BAT)->anim_index + 3;
				item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
				item->current_anim_state = 4;
				item->goal_anim_state = 4;
			}
		}
	} else {
		if(item->ai_bits) {
			GetAITarget(bat);
		} else {
			max_dist = infinite_distance;

			for(int i = 0; i < 5; i++) {
				baddie = &baddie_slots[i];

				if(baddie->item_num != NO_ITEM && baddie->item_num != item_number) {
					item2 = GetItem(currentLevel, baddie->item_num);

					if(item2->object_number == VON_CROY) {
						dx = item2->pos.pos.x - item->pos.pos.x;
						dz = item2->pos.pos.z - item->pos.pos.z;
						dist = SQUARE(dx) + SQUARE(dz);

						if(dist < max_dist) {
							bat->enemy = item2;
							max_dist = dist;
						}
					}
				}
			}
		}

		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 0);

		if(bat->flags) {
			bat->mood = ESCAPE_MOOD;
		}

		CreatureMood(item, &info, 0);
		angle = CreatureTurn(item, 3640);

		switch(item->current_anim_state) {
		case 2:

			if(info.distance < 0x10000 || !(GetRandomControl() & 0x3F)) {
				bat->flags = 0;
			}

			if(!bat->flags && (item->touch_bits || bat->enemy != lara_item && info.distance < 0x10000 && info.ahead && abs(item->pos.pos.y - bat->enemy->pos.pos.y) < 896)) {
				item->goal_anim_state = 3;
			}

			break;

		case 3:

			if(!bat->flags && item->touch_bits || bat->enemy != lara_item && info.distance < 0x10000 && info.ahead && abs(item->pos.pos.y - bat->enemy->pos.pos.y) < 896) {
				CreatureEffect(item, &bat_bite, DoBloodSplat);

				if(bat->enemy == lara_item) {
					lara_item->hit_points -= 2;
					lara_item->hit_status = 1;
				}

				bat->flags = 1;
			} else {
				item->goal_anim_state = 2;
				bat->mood = BORED_MOOD;
			}

			break;

		case 6:

			if(info.distance < 0x1900000 || item->hit_status || bat->hurt_by_lara) {
				item->goal_anim_state = 1;
			}

			break;
		}
	}

	CreatureAnimation(item_number, angle, 0);
}
