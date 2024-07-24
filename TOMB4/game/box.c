
#include "game/box.h"
#include "distances.h"
#include "game/aibits.h"
#include "game/aiinfo.h"
#include "game/aiobject.h"
#include "game/animstruct.h"
#include "game/biteinfo.h"
#include "game/boxinfo.h"
#include "game/boxnode.h"
#include "game/camera.h"
#include "game/control.h"
#include "game/creatureinfo.h"
#include "game/draw.h"
#include "game/effect2.h"
#include "game/floorinfo.h"
#include "game/heighttypes.h"
#include "game/itemflags.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/itemstatus.h"
#include "game/lara.h"
#include "game/lara_states.h"
#include "game/laragunstatus.h"
#include "game/larainfo.h"
#include "game/larawaterstatus.h"
#include "game/levelinfo.h"
#include "game/lot.h"
#include "game/lotinfo.h"
#include "game/moodtype.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/phd3dpos.h"
#include "game/roomflags.h"
#include "game/roominfo.h"
#include "game/rope.h"
#include "game/sphere.h"
#include "game/tomb4fx.h"
#include "game/weapontypes.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/file.h"
#include "specific/function_stubs.h"
#include <stdlib.h>


void CreatureDie(short item_number, long explode) {
	ITEM_INFO* item;
	ITEM_INFO* pickup;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	short pickup_number, room_number;
	short bounds[6];
	item = GetItem(currentLevel, item_number);
	item->hit_points = -16384;
	item->collidable = 0;

	if(explode) {
		if(GetObjectInfo(currentLevel, item->object_number)->HitEffect == 1) {
			ExplodingDeath2(item_number, -1, 258);
		} else {
			ExplodingDeath2(item_number, -1, 256);
		}

		KillItem(item_number);
	} else {
		RemoveActiveItem(item_number);
	}

	DisableBaddieAI(item_number);
	item->flags |= IFL_INVISIBLE | IFL_CLEARBODY;

	// inlined DropBaddyPickups..

	pickup_number = item->carried_item;

	while(pickup_number != NO_ITEM) {
		pickup = GetItem(currentLevel, pickup_number);

		if(item->object_number == TROOPS && item->trigger_flags == 1) {
			pickup->pos.pos.x = ((item->pos.pos.x + ((1024 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT)) & -512) | 512;
			pickup->pos.pos.z = ((item->pos.pos.z + ((1024 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT)) & -512) | 512;
		} else {
			pickup->pos.pos.x = (item->pos.pos.x & -512) | 512;
			pickup->pos.pos.z = (item->pos.pos.z & -512) | 512;
		}

		room_number = item->room_number;
		pickup->pos.pos.y = GetHeight(GetFloor(pickup->pos.pos.x, item->pos.pos.y, pickup->pos.pos.z, &room_number), pickup->pos.pos.x, item->pos.pos.y, pickup->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
		GetBoundsAccurate(pickup, &bounds[0]);
		pickup->pos.pos.y -= bounds[3];
		ItemNewRoom(pickup_number, item->room_number);
		pickup->flags |= IFL_TRIGGERED;
		pickup_number = pickup->carried_item;
	}
}

void InitialiseCreature(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);
	item->collidable = 1;
	item->data = NULL;
}

long CreatureActive(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(item->flags & IFL_CLEARBODY) {
		return 0;
	}

	if(item->status == ITEM_INVISIBLE) {
		if(EnableBaddieAI(item_number, 0)) {
			item->status = ITEM_ACTIVE;
		} else {
			return 0;
		}
	}

	return 1;
}

void CreatureAIInfo(ITEM_INFO* item, AI_INFO* info) {
	CREATURE_INFO* creature;
	OBJECT_INFO* obj;
	ITEM_INFO* enemy;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	short* zone;
	long x, y, z;
	short pivot, ang, state;

	creature = (CREATURE_INFO*)item->data;

	if(!creature) {
		return;
	}

	obj = GetObjectInfo(currentLevel, item->object_number);

	if(item->poisoned) {
		if(!obj->undead && !(wibble & 0x3F) && item->hit_points > 1) {
			item->hit_points--;
		}
	}

	enemy = creature->enemy;

	if(!enemy) {
		enemy = lara_item;
		creature->enemy = lara_item;
	}

	zone = GetZone(currentLevel, creature->LOT.zone, flip_status);
	r = GetRoom(currentLevel, item->room_number);
	floor = &r->floor[((item->pos.pos.z - r->z) >> 10) + r->x_size * ((item->pos.pos.x - r->x) >> 10)];
	item->box_number = floor->box;
	info->zone_number = zone[item->box_number];

	r = GetRoom(currentLevel, enemy->room_number);
	floor = &r->floor[((enemy->pos.pos.z - r->z) >> 10) + r->x_size * ((enemy->pos.pos.x - r->x) >> 10)];
	enemy->box_number = floor->box;
	info->enemy_zone = zone[enemy->box_number];

	if(GetBox(currentLevel, enemy->box_number)->overlap_index & creature->LOT.block_mask
	   || creature->LOT.node[item->box_number].search_number == (creature->LOT.search_number | 0x8000)) {
		info->enemy_zone |= 0x4000;
	}

	pivot = obj->pivot_length;

	if(enemy == lara_item) {
		ang = lara.move_angle;
	} else {
		ang = enemy->pos.y_rot;
	}

	x = enemy->pos.pos.x + (14 * enemy->speed * phd_sin(ang) >> W2V_SHIFT) - (pivot * phd_sin(item->pos.y_rot) >> W2V_SHIFT) - item->pos.pos.x;
	y = item->pos.pos.y - enemy->pos.pos.y;
	z = enemy->pos.pos.z + (14 * enemy->speed * phd_cos(ang) >> W2V_SHIFT) - (pivot * phd_cos(item->pos.y_rot) >> W2V_SHIFT) - item->pos.pos.z;

	ang = (short)phd_atan(z, x);

	if(z > 32000 || z < -32000 || x > 32000 || x < -32000) {
		info->distance = infinite_distance;
	} else if(creature->enemy) {
		info->distance = SQUARE(x) + SQUARE(z);
	} else {
		info->distance = infinite_distance;
	}

	info->angle = ang - item->pos.y_rot;
	info->enemy_facing = ang - enemy->pos.y_rot + 0x8000;

	x = abs(x);
	z = abs(z);

	if(enemy == lara_item) {
		state = lara_item->current_anim_state;

		if(state == AS_DUCK || state == AS_DUCKROLL || state == AS_ALL4S || state == AS_CRAWL || state == AS_ALL4TURNL || state == AS_ALL4TURNR || state == AS_DUCKROTL || state == AS_DUCKROTR) {
			y -= 384;
		}
	}

	if(x > z) {
		info->x_angle = (short)phd_atan(x + (z >> 1), y);
	} else {
		info->x_angle = (short)phd_atan(z + (x >> 1), y);
	}

	if(info->angle > -0x4000 && info->angle < 0x4000) {
		info->ahead = 1;
	} else {
		info->ahead = 0;
	}

	if(info->ahead && enemy->hit_points > 0 && abs(enemy->pos.pos.y - item->pos.pos.y) <= 512) {
		info->bite = 1;
	} else {
		info->bite = 0;
	}
}

long SearchLOT(LOT_INFO* LOT, long expansion) {
	BOX_NODE* node;
	BOX_NODE* expand;
	BOX_INFO* box;
	short* zone;
	long index, done, box_number, overlap_flags, change;
	short search_zone;

	zone = GetZone(currentLevel, LOT->zone, flip_status);
	search_zone = zone[LOT->head];

	for(int i = 0; i < expansion; i++) {
		if(LOT->head == NO_BOX) {
			LOT->tail = NO_BOX;
			return 0;
		}

		box = GetBox(currentLevel, LOT->head);
		node = &LOT->node[LOT->head];
		index = box->overlap_index & 0x3FFF;
		done = 0;

		do {
			box_number = *GetOverlap(currentLevel, index);
			index++;
			overlap_flags = box_number & ~NO_BOX;

			if(box_number & 0x8000) {
				done = 1;
			}

			box_number &= NO_BOX;

			if(!LOT->fly && search_zone != zone[box_number]) { // zone isn't in search area + can't fly
				continue;
			}

			change = GetBox(currentLevel, box_number)->height - box->height;

			if((change > LOT->step || change < LOT->drop) && (!(overlap_flags & 0x2000) || !LOT->can_monkey)) { // can't traverse block + can't monkey
				continue;
			}

			if(overlap_flags & 0x800 && !LOT->can_jump) { // can't jump
				continue;
			}

			expand = &LOT->node[box_number];

			if((node->search_number & 0x7FFF) < (expand->search_number & 0x7FFF)) {
				continue;
			}

			if(node->search_number & 0x8000) {
				if((node->search_number & 0x7FFF) == (expand->search_number & 0x7FFF)) {
					continue;
				}

				expand->search_number = node->search_number;
			} else {
				if((node->search_number & 0x7FFF) == (expand->search_number & 0x7FFF) && !(expand->search_number & 0x8000)) {
					continue;
				}

				if(GetBox(currentLevel, box_number)->overlap_index & LOT->block_mask) {
					expand->search_number = node->search_number | 0x8000;
				} else {
					expand->search_number = node->search_number;
					expand->exit_box = LOT->head;
				}
			}

			if(expand->next_expansion == NO_BOX && box_number != LOT->tail) {
				LOT->node[LOT->tail].next_expansion = (short)box_number;
				LOT->tail = (short)box_number;
			}

		} while(!done);

		LOT->head = node->next_expansion;
		node->next_expansion = NO_BOX;
	}

	return 1;
}

long UpdateLOT(LOT_INFO* LOT, long expansion) {
	BOX_NODE* expand;

	if(LOT->required_box != NO_BOX && LOT->required_box != LOT->target_box) {
		LOT->target_box = LOT->required_box;
		expand = &LOT->node[LOT->required_box];

		if(expand->next_expansion == NO_BOX && LOT->tail != LOT->required_box) {
			expand->next_expansion = LOT->head;

			if(LOT->head == NO_BOX) {
				LOT->tail = LOT->target_box;
			}

			LOT->head = LOT->target_box;
		}

		LOT->search_number++;
		expand->search_number = LOT->search_number;
		expand->exit_box = NO_BOX;
	}

	return SearchLOT(LOT, expansion);
}

void TargetBox(LOT_INFO* LOT, short box_number) {
	BOX_INFO* box;

	box = GetBox(currentLevel, box_number & 0x7FF);
	LOT->target.x = (((unsigned long)box->bottom - (unsigned long)box->top - 1) >> 5) * GetRandomControl() + ((unsigned long)box->top << 10) + 512;
	LOT->target.z = (((unsigned long)box->right - (unsigned long)box->left - 1) >> 5) * GetRandomControl() + ((unsigned long)box->left << 10) + 512;
	LOT->required_box = box_number & 0x7FF;

	if(LOT->fly) {
		LOT->target.y = box->height - 384;
	} else {
		LOT->target.y = box->height;
	}
}

long EscapeBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number) {
	BOX_INFO* box;
	long x, z;

	box = GetBox(currentLevel, box_number);
	x = (((unsigned long)box->bottom + (unsigned long)box->top) << 9) - enemy->pos.pos.x;
	z = (((unsigned long)box->left + (unsigned long)box->right) << 9) - enemy->pos.pos.z;

	if(x > -5120 && x < 5120 && z > -5120 && z < 5120) {
		return 0;
	}

	return (z > 0) == (item->pos.pos.z > enemy->pos.pos.z) || (x > 0) == (item->pos.pos.x > enemy->pos.pos.x);
}

long ValidBox(ITEM_INFO* item, short zone_number, short box_number) {
	CREATURE_INFO* creature;
	BOX_INFO* box;

	creature = (CREATURE_INFO*)item->data;

	if(!creature->LOT.fly && GetZone(currentLevel, creature->LOT.zone, flip_status)[box_number] != zone_number) {
		return 0;
	}

	box = GetBox(currentLevel, box_number);

	if(creature->LOT.block_mask & box->overlap_index) {
		return 0;
	}

	if(item->pos.pos.z > box->left << 10 && item->pos.pos.z < box->right << 10 && item->pos.pos.x > box->top << 10 && item->pos.pos.x < box->bottom << 10) {
		return 0;
	}

	return 1;
}

long StalkBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number) {
	BOX_INFO* box;
	long x, z, xrange, zrange, enemy_quad, box_quad, baddie_quad;

	if(!enemy) {
		return 0;
	}

	box = GetBox(currentLevel, box_number);
	x = (((unsigned long)box->bottom + (unsigned long)box->top) << 9) - enemy->pos.pos.x;
	z = (((unsigned long)box->left + (unsigned long)box->right) << 9) - enemy->pos.pos.z;
	xrange = ((unsigned long)box->bottom - (unsigned long)box->top + 3) << 10; // 3 is the # of blocks
	zrange = ((unsigned long)box->right - (unsigned long)box->left + 3) << 10;

	if(x > xrange || x < -xrange || z > zrange || z < -zrange) {
		return 0;
	}

	enemy_quad = (enemy->pos.y_rot >> W2V_SHIFT) + 2;
	box_quad = z <= 0 ? (x <= 0 ? 0 : 3) : (x > 0) + 1;

	if(enemy_quad == box_quad) {
		return 0;
	}

	baddie_quad = item->pos.pos.z <= enemy->pos.pos.z ? ((item->pos.pos.x <= enemy->pos.pos.x) ? 0 : 3) : (item->pos.pos.x > enemy->pos.pos.x) + 1;
	return enemy_quad != baddie_quad || abs(enemy_quad - box_quad) != 2;
}

target_type CalculateTarget(PHD_VECTOR* target, ITEM_INFO* item, LOT_INFO* LOT) {
	BOX_INFO* box;
	long box_number, box_left, box_right, box_top, box_bottom;
	long left, right, top, bottom, prime_free;

	UpdateLOT(LOT, 5);
	target->x = item->pos.pos.x;
	target->y = item->pos.pos.y;
	target->z = item->pos.pos.z;
	box_number = item->box_number;

	if(box_number == NO_BOX) {
		return NO_TARGET;
	}

	box = GetBox(currentLevel, box_number);
	left = box->left << 10;
	right = (box->right << 10) - 1;
	top = box->top << 10;
	bottom = (box->bottom << 10) - 1;
	prime_free = 15;

	do {
		box = GetBox(currentLevel, box_number);

		if(LOT->fly) {
			if(target->y > box->height - 1024) {
				target->y = box->height - 1024;
			}
		} else if(target->y > box->height) {
			target->y = box->height;
		}

		box_left = box->left << 10;
		box_right = (box->right << 10) - 1;
		box_top = box->top << 10;
		box_bottom = (box->bottom << 10) - 1;

		if(item->pos.pos.z >= box_left && item->pos.pos.z <= box_right && item->pos.pos.x >= box_top && item->pos.pos.x <= box_bottom) {
			left = box->left << 10;
			right = (box->right << 10) - 1;
			top = box->top << 10;
			bottom = (box->bottom << 10) - 1;
		} else {
			if(item->pos.pos.z < box_left && prime_free != 2) {
				if(prime_free & 1 && item->pos.pos.x >= box_top && item->pos.pos.x <= box_bottom) {
					if(target->z < box_left + 512) {
						target->z = box_left + 512;
					}

					if(prime_free & 16) {
						return SECONDARY_TARGET;
					}

					if(box_top > top) {
						top = box_top;
					}

					if(box_bottom < bottom) {
						bottom = box_bottom;
					}

					prime_free = 1;
				} else if(prime_free != 1) {
					target->z = right - 512;

					if(prime_free != 15) {
						return SECONDARY_TARGET;
					}

					prime_free = 31;
				}
			} else if(item->pos.pos.z > box_right && prime_free != 1) {
				if(prime_free & 2 && item->pos.pos.x >= box_top && item->pos.pos.x <= box_bottom) {
					if(target->z > box_right - 512) {
						target->z = box_right - 512;
					}

					if(prime_free & 16) {
						return SECONDARY_TARGET;
					}

					if(box_top > top) {
						top = box_top;
					}

					if(box_bottom < bottom) {
						bottom = box_bottom;
					}

					prime_free = 2;
				} else if(prime_free != 2) {
					target->z = left + 512;

					if(prime_free != 15) {
						return SECONDARY_TARGET;
					}

					prime_free = 31;
				}
			}

			if(item->pos.pos.x < box_top && prime_free != 8) {
				if(prime_free & 4 && item->pos.pos.z >= box_left && item->pos.pos.z <= box_right) {
					if(target->x < box_top + 512) {
						target->x = box_top + 512;
					}

					if(prime_free & 16) {
						return SECONDARY_TARGET;
					}

					if(box_left > left) {
						left = box_left;
					}

					if(box_right < right) {
						right = box_right;
					}

					prime_free = 4;
				} else if(prime_free != 4) {
					target->x = bottom - 512;

					if(prime_free != 15) {
						return SECONDARY_TARGET;
					}

					prime_free = 31;
				}
			} else if(item->pos.pos.x > box_bottom && prime_free != 4) {
				if(prime_free & 8 && item->pos.pos.z >= box_left && item->pos.pos.z <= box_right) {
					if(target->x > box_bottom - 512) {
						target->x = box_bottom - 512;
					}

					if(prime_free & 16) {
						return SECONDARY_TARGET;
					}

					if(box_left > left) {
						left = box_left;
					}

					if(box_right < right) {
						right = box_right;
					}

					prime_free = 8;
				} else if(prime_free != 8) {
					target->x = top + 512;

					if(prime_free != 15) {
						return SECONDARY_TARGET;
					}

					prime_free = 31;
				}
			}
		}

		if(box_number == LOT->target_box) {
			if(prime_free & 3) {
				target->z = LOT->target.z;
			} else if(!(prime_free & 16)) {
				if(target->z < box_left + 512) {
					target->z = box_left + 512;
				} else if(target->z > box_right - 512) {
					target->z = box_right - 512;
				}
			}

			if(prime_free & 12) {
				target->x = LOT->target.x;
			} else if(!(prime_free & 16)) {
				if(target->x < box_top + 512) {
					target->x = box_top + 512;
				} else if(target->x > box_bottom - 512) {
					target->x = box_bottom - 512;
				}
			}

			target->y = LOT->target.y;
			return PRIME_TARGET;
		}

		box_number = LOT->node[box_number].exit_box;

		if(box_number != NO_BOX && GetBox(currentLevel, box_number)->overlap_index & LOT->block_mask) {
			break;
		}
	} while(box_number != NO_BOX);

	if(!(prime_free & 16)) {
		if(target->z < box_left + 512) {
			target->z = box_left + 512;
		} else if(target->z > box_right - 512) {
			target->z = box_right - 521;
		}
	}

	if(!(prime_free & 16)) // wut
	{
		if(target->x < box_top + 512) {
			target->x = box_top + 512;
		} else if(target->x > box_bottom - 512) {
			target->x = box_bottom - 512;
		}
	}

	if(LOT->fly) {
		target->y = box->height - 320;
	} else {
		target->y = box->height;
	}

	return NO_TARGET;
}

void CreatureMood(ITEM_INFO* item, AI_INFO* info, long violent) {
	CREATURE_INFO* creature;
	ITEM_INFO* enemy;
	LOT_INFO* LOT;
	/* static */ target_type type;
	short index, box_no;

	creature = (CREATURE_INFO*)item->data;

	if(!creature) {
		return;
	}

	enemy = creature->enemy;
	LOT = &creature->LOT;

	switch(creature->mood) {
	case BORED_MOOD:
		box_no = LOT->node[(creature->LOT.zone_count * GetRandomControl()) >> 15].box_number;

		if(ValidBox(item, info->zone_number, box_no)) {
			if(StalkBox(item, enemy, box_no) && enemy->hit_points > 0 && creature->enemy) {
				TargetBox(&creature->LOT, box_no);
				creature->mood = BORED_MOOD;
			} else if(creature->LOT.required_box == NO_BOX) {
				TargetBox(&creature->LOT, box_no);
			}
		}

		break;

	case ATTACK_MOOD:
		creature->LOT.target.x = enemy->pos.pos.x;
		creature->LOT.target.y = enemy->pos.pos.y;
		creature->LOT.target.z = enemy->pos.pos.z;
		creature->LOT.required_box = enemy->box_number;

		if(creature->LOT.fly && lara.water_status == LW_ABOVE_WATER) {
			creature->LOT.target.y += GetBestFrame(enemy)[2];
		}

		break;

	case ESCAPE_MOOD:
		box_no = LOT->node[(creature->LOT.zone_count * GetRandomControl()) >> 15].box_number;

		if(ValidBox(item, info->zone_number, box_no) && creature->LOT.required_box == NO_BOX) {
			if(EscapeBox(item, enemy, box_no)) {
				TargetBox(&creature->LOT, box_no);
			} else if(info->zone_number == info->enemy_zone && StalkBox(item, enemy, box_no) && !violent) {
				TargetBox(&creature->LOT, box_no);
				creature->mood = STALK_MOOD;
			}
		}

		break;

	case STALK_MOOD:

		if(creature->LOT.required_box == NO_BOX || !StalkBox(item, enemy, creature->LOT.required_box)) {
			box_no = LOT->node[(creature->LOT.zone_count * GetRandomControl()) >> 15].box_number;

			if(ValidBox(item, info->zone_number, box_no)) {
				if(StalkBox(item, enemy, box_no)) {
					TargetBox(&creature->LOT, box_no);
				} else if(creature->LOT.required_box == NO_BOX) {
					TargetBox(&creature->LOT, box_no);

					if(info->zone_number != info->enemy_zone) {
						creature->mood = BORED_MOOD;
					}
				}
			}
		}

		break;
	}

	if(creature->LOT.target_box == NO_BOX) {
		TargetBox(&creature->LOT, item->box_number);
	}

	type = CalculateTarget(&creature->target, item, &creature->LOT);
	creature->jump_ahead = 0;
	creature->monkey_ahead = 0;

	if(LOT->node[item->box_number].exit_box != NO_BOX) {
		index = GetBox(currentLevel, item->box_number)->overlap_index & 0x3FFF;

		do {
			box_no = *GetOverlap(currentLevel, index++);
		} while(box_no != NO_BOX && !(box_no & 0x8000) && (box_no & 0x7FF) != LOT->node[item->box_number].exit_box);

		if((box_no & 0x7FF) == LOT->node[item->box_number].exit_box) {
			if(box_no & 0x800) {
				creature->jump_ahead = 1;
			}

			if(box_no & 0x2000) {
				creature->monkey_ahead = 1;
			}
		}
	}
}

void GetCreatureMood(ITEM_INFO* item, AI_INFO* info, long violent) {
	CREATURE_INFO* creature;
	ITEM_INFO* enemy;
	LOT_INFO* LOT;
	mood_type mood;

	creature = (CREATURE_INFO*)item->data;

	if(!creature) {
		return;
	}

	LOT = &creature->LOT;
	enemy = creature->enemy;

	if(creature->LOT.node[item->box_number].search_number == (creature->LOT.search_number | 0x8000)) {
		creature->LOT.required_box = NO_BOX;
	}

	if(creature->mood != ATTACK_MOOD && creature->LOT.required_box != NO_BOX && !ValidBox(item, info->zone_number, creature->LOT.target_box)) {
		if(info->zone_number == info->enemy_zone) {
			creature->mood = BORED_MOOD;
		}

		creature->LOT.required_box = NO_BOX;
	}

	mood = creature->mood;

	if(enemy) {
		if(enemy->hit_points <= 0 && enemy == lara_item) {
			creature->mood = BORED_MOOD;
		} else if(violent) {
			switch(creature->mood) {
			case BORED_MOOD:
			case STALK_MOOD:

				if(info->zone_number == info->enemy_zone) {
					creature->mood = ATTACK_MOOD;
				} else if(item->hit_status) {
					creature->mood = ESCAPE_MOOD;
				}

				break;

			case ATTACK_MOOD:

				if(info->zone_number != info->enemy_zone) {
					creature->mood = BORED_MOOD;
				}

				break;

			case ESCAPE_MOOD:

				if(info->zone_number == info->enemy_zone) {
					creature->mood = ATTACK_MOOD;
				}

				break;
			}
		} else {
			switch(creature->mood) {
			case BORED_MOOD:
			case STALK_MOOD:

				if(creature->alerted && info->zone_number != info->enemy_zone) {
					creature->mood = info->distance > 3072 ? STALK_MOOD : BORED_MOOD;
				} else if(info->zone_number == info->enemy_zone) {
					if(info->distance < 0x900000 || mood == STALK_MOOD && LOT->required_box == NO_BOX) {
						creature->mood = ATTACK_MOOD;
					} else {
						creature->mood = STALK_MOOD;
					}
				}

				break;

			case ATTACK_MOOD:

				if((item->hit_status && GetRandomControl() < 2048) || info->zone_number != info->enemy_zone) {
					creature->mood = STALK_MOOD;
				} else if(info->zone_number != info->enemy_zone && info->distance > 6144) {
					creature->mood = BORED_MOOD;
				}

				break;

			case ESCAPE_MOOD:

				if(info->zone_number == info->enemy_zone && GetRandomControl() < 256) {
					creature->mood = STALK_MOOD;
				}

				break;
			}
		}
	} else {
		creature->mood = BORED_MOOD;
	}

	if(mood != creature->mood) {
		if(mood == ATTACK_MOOD) {
			TargetBox(LOT, LOT->target_box);
			LOT = &creature->LOT;
		}

		LOT->required_box = NO_BOX;
	}
}

long CreatureCreature(short item_number) {
	ITEM_INFO* item;
	long x, z, dx, dz, dist;
	short yrot, rad, item_num;

	item = GetItem(currentLevel, item_number);
	x = item->pos.pos.x;
	z = item->pos.pos.z;
	yrot = item->pos.y_rot;
	rad = GetObjectInfo(currentLevel, item->object_number)->radius;

	for(item_num = GetRoom(currentLevel, item->room_number)->item_number; item_num != NO_ITEM; item_num = item->next_item) {
		item = GetItem(currentLevel, item_num);

		if(item_num != item_number && item != lara_item && item->status == ITEM_ACTIVE && item->hit_points > 0) {
			dx = abs(item->pos.pos.x - x);
			dz = abs(item->pos.pos.z - z);
			dist = dx > dz ? dx + (dz >> 1) : dz + (dx >> 1);

			if(dist < rad + GetObjectInfo(currentLevel, item->object_number)->radius) {
				return (short)(phd_atan(item->pos.pos.z - z, item->pos.pos.x - x) - yrot);
			}
		}
	}

	return 0;
}

long BadFloor(long x, long y, long z, long box_height, long next_height, short room_number, LOT_INFO* LOT) {
	FLOOR_INFO* floor;
	BOX_INFO* box;

	floor = GetFloor(x, y, z, &room_number);

	if(floor->box == NO_BOX) {
		return 1;
	}

	if(LOT->is_jumping) {
		return 0;
	}

	box = GetBox(currentLevel, floor->box);

	if(box->overlap_index & LOT->block_mask) {
		return 1;
	}

	if(box_height - box->height > LOT->step || box_height - box->height < LOT->drop) {
		return 1;
	}

	if(box_height - box->height < -LOT->step && box->height > next_height) {
		return 1;
	}

	if(LOT->fly && y > box->height + LOT->fly) {
		return 1;
	}

	return 0;
}

long CreatureAnimation(short item_number, short angle, short tilt) {
	ITEM_INFO* item;
	CREATURE_INFO* creature;
	LOT_INFO* LOT;
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	PHD_VECTOR oldPos;
	short* zone;
	short bounds[6];
	long box_height, y, height, next_box, next_height, x, z, wx, wz, xShift, zShift, dy;
	short room_number, rad;

	item = GetItem(currentLevel, item_number);
	creature = (CREATURE_INFO*)item->data;

	if(!creature) {
		return 0;
	}

	LOT = &creature->LOT;
	oldPos.x = item->pos.pos.x;
	oldPos.y = item->pos.pos.y;
	oldPos.z = item->pos.pos.z;
	height = GetBox(currentLevel, item->box_number)->height;
	zone = GetZone(currentLevel, LOT->zone, flip_status);
	AnimateItem(item);

	if(item->status == ITEM_DEACTIVATED) {
		CreatureDie(item_number, 0);
		return 0;
	}

	GetBoundsAccurate(item, bounds);
	y = item->pos.pos.y + bounds[2];
	room_number = item->room_number;
	GetFloor(oldPos.x, y, oldPos.z, &room_number);
	floor = GetFloor(item->pos.pos.x, y, item->pos.pos.z, &room_number);
	box_height = GetBox(currentLevel, floor->box)->height;
	next_box = LOT->node[floor->box].exit_box;

	if(next_box == NO_BOX) {
		next_height = GetBox(currentLevel, floor->box)->height;
	} else {
		next_height = GetBox(currentLevel, next_box)->height;
	}

	if(floor->box == NO_BOX || !LOT->is_jumping && (zone[item->box_number] != zone[floor->box] || height - box_height > LOT->step || height - box_height < LOT->drop)) {
		wx = item->pos.pos.x >> 10;
		wz = item->pos.pos.z >> 10;

		if(wx < oldPos.x >> 10) {
			item->pos.pos.x = oldPos.x & ~0x3FF;
		} else {
			item->pos.pos.x = oldPos.x | 0x3FF;
		}

		if(wz < oldPos.z >> 10) {
			item->pos.pos.z = oldPos.z & ~0x3FF;
		} else {
			item->pos.pos.z = oldPos.z | 0x3FF;
		}

		floor = GetFloor(item->pos.pos.x, y, item->pos.pos.z, &room_number);
		box_height = GetBox(currentLevel, floor->box)->height;
		next_box = LOT->node[floor->box].exit_box;

		if(next_box == NO_BOX) {
			next_height = GetBox(currentLevel, floor->box)->height;
		} else {
			next_height = GetBox(currentLevel, next_box)->height;
		}
	}

	x = item->pos.pos.x;
	z = item->pos.pos.z;
	wx = x & 0x3FF;
	wz = z & 0x3FF;
	rad = GetObjectInfo(currentLevel, item->object_number)->radius;
	xShift = 0;
	zShift = 0;

	if(wz < rad) {
		if(BadFloor(x, y, z - rad, box_height, next_height, room_number, LOT)) {
			zShift = rad - wz;
		}

		if(wx < rad) {
			if(!BadFloor(x - rad, y, z, box_height, next_height, room_number, LOT)) {
				if(!zShift && BadFloor(x - rad, y, z - rad, box_height, next_height, room_number, LOT)) {
					if(item->pos.y_rot > -0x6000 && item->pos.y_rot < 0x2000) {
						zShift = rad - wz;
					} else {
						xShift = rad - wx;
					}
				}
			} else {
				xShift = rad - wx;
			}
		} else if(wx > 1024 - rad) {
			if(!BadFloor(x + rad, y, z, box_height, next_height, room_number, LOT)) {
				if(!zShift && BadFloor(x + rad, y, z - rad, box_height, next_height, room_number, LOT)) {
					if(item->pos.y_rot > -0x2000 && item->pos.y_rot < 0x6000) {
						zShift = rad - wz;
					} else {
						xShift = 1024 - rad - wx;
					}
				}
			} else {
				xShift = 1024 - rad - wx;
			}
		}
	} else if(wz > 1024 - rad) {
		if(BadFloor(x, y, z + rad, box_height, next_height, room_number, LOT)) {
			zShift = 1024 - rad - wz;
		}

		if(wx < rad) {
			if(!BadFloor(x - rad, y, z, box_height, next_height, room_number, LOT)) {
				if(!zShift && BadFloor(x - rad, y, z + rad, box_height, next_height, room_number, LOT)) {
					if(item->pos.y_rot > -0x2000 && item->pos.y_rot < 0x6000) {
						xShift = rad - wx;
					} else {
						zShift = 1024 - rad - wz;
					}
				}
			} else {
				xShift = rad - wx;
			}

		} else if(wx > 1024 - rad) {
			if(!BadFloor(x + rad, y, z, box_height, next_height, room_number, LOT)) {
				if(!zShift && BadFloor(x + rad, y, z + rad, box_height, next_height, room_number, LOT)) {
					if(item->pos.y_rot > -0x6000 && item->pos.y_rot < 0x2000) {
						xShift = 1024 - rad - wx;
					} else {
						zShift = 1024 - rad - wz;
					}
				}
			} else {
				xShift = 1024 - rad - wx;
			}
		}
	} else if(wx < rad) {
		if(BadFloor(x - rad, y, z, box_height, next_height, room_number, LOT)) {
			xShift = rad - wx;
		}
	} else if(wx > 1024 - rad) {
		if(BadFloor(x + rad, y, z, box_height, next_height, room_number, LOT)) {
			xShift = 1024 - rad - wx;
		}
	}

	item->pos.pos.x += xShift;
	item->pos.pos.z += zShift;

	if(xShift || zShift) {
		floor = GetFloor(item->pos.pos.x, y, item->pos.pos.z, &room_number);
		item->pos.y_rot += angle;

		if(tilt) {
			CreatureTilt(item, 2 * tilt);
		}
	}

	if(item->speed && item->hit_points > 0) {
		angle = (short)CreatureCreature(item_number);

		if(angle) {
			if(abs(angle) < 2048) {
				item->pos.y_rot -= angle;
			} else if(angle > 0) {
				item->pos.y_rot -= 2048;
			} else {
				item->pos.y_rot += 2048;
			}

			return 1;
		}
	}

	if(LOT->fly && item->hit_points > 0) {
		dy = creature->target.y - item->pos.pos.y;

		if(dy > LOT->fly) {
			dy = LOT->fly;
		} else if(dy < -LOT->fly) {
			dy = -LOT->fly;
		}

		height = GetHeight(floor, item->pos.pos.x, y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

		if(item->pos.pos.y + dy > height) {
			if(item->pos.pos.y > height) {
				dy = -LOT->fly;
				item->pos.pos.x = oldPos.x;
				item->pos.pos.z = oldPos.z;
			} else {
				dy = 0;
				item->pos.pos.y = height;
			}
		} else if(GetObjectInfo(currentLevel, item->object_number)->water_creature) {
			height = GetCeiling(floor, item->pos.pos.x, y, item->pos.pos.z);

			if(item->pos.pos.y + bounds[2] + dy < height) {
				if(item->pos.pos.y + bounds[2] < height) {
					dy = LOT->fly;
					item->pos.pos.x = oldPos.x;
					item->pos.pos.z = oldPos.z;
				} else {
					dy = 0;
				}
			}
		} else {
			GetFloor(item->pos.pos.x, y + 256, item->pos.pos.z, &room_number);

			if(GetRoom(currentLevel, room_number)->flags & ROOM_UNDERWATER) {
				dy = -LOT->fly;
			}
		}

		item->pos.pos.y += dy;
		floor = GetFloor(item->pos.pos.x, y, item->pos.pos.z, &room_number);
		item->floor = GetHeight(floor, item->pos.pos.x, y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

		if(item->speed) {
			angle = (short)phd_atan(item->speed, -dy);

			if(angle < -3640) {
				angle = -3640;
			} else if(angle > 3640) {
				angle = 3640;
			}
		} else {
			angle = 0;
		}

		if(angle < item->pos.x_rot - 182) {
			item->pos.x_rot -= 182;
		} else if(angle > item->pos.x_rot + 182) {
			item->pos.x_rot += 182;
		} else {
			item->pos.x_rot = angle;
		}
	} else if(LOT->is_jumping) {
		floor = GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);
		item->floor = GetHeight(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

		if(LOT->is_monkeying) {
			item->pos.pos.y = GetCeiling(floor, item->pos.pos.x, y, item->pos.pos.z) - bounds[2];
		} else if(item->pos.pos.y > item->floor) {
			if(item->pos.pos.y > item->floor + 256) {
				item->pos.pos.x = oldPos.x;
				item->pos.pos.z = oldPos.z;
				item->pos.pos.y = oldPos.y;
			} else {
				item->pos.pos.y = item->floor;
			}
		}
	} else {
		floor = GetFloor(item->pos.pos.x, y, item->pos.pos.z, &room_number);
		height = GetCeiling(floor, item->pos.pos.x, y, item->pos.pos.z);

		if(item->pos.pos.y + bounds[2] < height) {
			item->pos.pos.x = oldPos.x;
			item->pos.pos.z = oldPos.z;
			item->pos.pos.y = oldPos.y;
		}

		floor = GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);
		item->floor = GetHeight(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

		if(item->pos.pos.y > item->floor) {
			item->pos.pos.y = item->floor;
		} else if(item->floor - item->pos.pos.y > 64) {
			item->pos.pos.y += 64;
		} else if(item->pos.pos.y < item->floor) {
			item->pos.pos.y = item->floor;
		}
	}

	room_number = item->room_number;
	GetFloor(item->pos.pos.x, item->pos.pos.y - 32, item->pos.pos.z, &room_number);

	if(item->room_number != room_number) {
		ItemNewRoom(item_number, room_number);
	}

	return 1;
}

short CreatureTurn(ITEM_INFO* item, short maximum_turn) {
	CREATURE_INFO* creature;
	ROOM_INFO* r;
	long x, z, feelxplus, feelzplus, feelxminus, feelzminus, feelxmid, feelzmid, feelplus, feelminus, feelmid;
	short angle;

	creature = (CREATURE_INFO*)item->data;

	if(!creature || !maximum_turn) {
		return 0;
	}

	x = item->pos.pos.x;
	z = item->pos.pos.z;
	r = GetRoom(currentLevel, item->room_number);

	feelxplus = x + (512 * phd_sin(item->pos.y_rot + 8190) >> W2V_SHIFT);
	feelzplus = z + (512 * phd_cos(item->pos.y_rot + 8190) >> W2V_SHIFT);
	feelplus = r->floor[((feelzplus - r->z) >> 10) + r->x_size * ((feelxplus - r->x) >> 10)].stopper;

	feelxminus = x + (512 * phd_sin(item->pos.y_rot - 8190) >> W2V_SHIFT);
	feelzminus = z + (512 * phd_cos(item->pos.y_rot - 8190) >> W2V_SHIFT);
	feelminus = r->floor[((feelzminus - r->z) >> 10) + r->x_size * ((feelxminus - r->x) >> 10)].stopper;

	feelxmid = x + (512 * phd_sin(item->pos.y_rot) >> W2V_SHIFT);
	feelzmid = z + (512 * phd_cos(item->pos.y_rot) >> W2V_SHIFT);
	feelmid = r->floor[((feelzmid - r->z) >> 10) + r->x_size * ((feelxmid - r->x) >> 10)].stopper;

	if(feelminus && feelmid) {
		creature->target.x = feelxplus;
		creature->target.z = feelzplus;
	} else if(feelplus && feelmid) {
		creature->target.x = feelxminus;
		creature->target.z = feelzminus;
	} else if(feelplus || feelminus) {
		creature->target.x = feelxmid;
		creature->target.z = feelzmid;
	}

	x = creature->target.x - item->pos.pos.x;
	z = creature->target.z - item->pos.pos.z;
	angle = (short)(phd_atan(z, x) - item->pos.y_rot);

	if(angle > 0x4000 || angle < -0x4000) {
		if(SQUARE(x) + SQUARE(z) < SQUARE((item->speed << 14) / maximum_turn)) {
			maximum_turn >>= 1;
		}
	}

	if(angle > maximum_turn) {
		angle = maximum_turn;
	} else if(angle < -maximum_turn) {
		angle = -maximum_turn;
	}

	item->pos.y_rot += angle;
	return angle;
}

void CreatureTilt(ITEM_INFO* item, short angle) {
	angle = (angle << 2) - item->pos.z_rot;

	if(angle < -546) {
		item->pos.z_rot -= 546;
	} else if(angle > 546) {
		item->pos.z_rot += 546;
	}
}

void CreatureJoint(ITEM_INFO* item, short joint, short required) {
	CREATURE_INFO* creature;
	short change;

	creature = (CREATURE_INFO*)item->data;

	if(!creature) {
		return;
	}

	change = required - creature->joint_rotation[joint];

	if(change > 546) {
		change = 546;
	} else if(change < -546) {
		change = -546;
	}

	creature->joint_rotation[joint] += change;

	if(creature->joint_rotation[joint] > 0x3000) {
		creature->joint_rotation[joint] = 0x3000;
	} else if(creature->joint_rotation[joint] < -0x3000) {
		creature->joint_rotation[joint] = -0x3000;
	}
}

void CreatureFloat(short item_number) {
	ITEM_INFO* item;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	long water_level;
	short room_number;

	item = GetItem(currentLevel, item_number);
	item->hit_points = -16384;
	item->pos.x_rot = 0;
	water_level = GetWaterHeight(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, item->room_number);

	if(item->pos.pos.y > water_level) {
		item->pos.pos.y -= 32;
	}

	if(item->pos.pos.y < water_level) {
		item->pos.pos.y = water_level;
	}

	AnimateItem(item);
	room_number = item->room_number;
	item->floor = GetHeight(GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number), item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

	if(item->room_number != room_number) {
		ItemNewRoom(item_number, room_number);
	}

	if(item->pos.pos.y <= water_level && item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base) {
		item->status = ITEM_DEACTIVATED;
		item->collidable = 0;
		item->pos.pos.y = water_level;
		DisableBaddieAI(item_number);
		RemoveActiveItem(item_number);
		item->after_death = 1;
	}
}

void CreatureUnderwater(ITEM_INFO* item, long depth) {
	long water_level, floorheight;
	short room_number;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;

	water_level = GetWaterHeight(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, item->room_number);

	if(item->pos.pos.y < water_level + depth) {
		room_number = item->room_number;
		floorheight = GetHeight(GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number), item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
		item->pos.pos.y = water_level + depth;

		if(item->pos.pos.y > floorheight) {
			item->pos.pos.y = floorheight;
		}

		if(item->pos.x_rot > 364) {
			item->pos.x_rot -= 364;
		} else if(item->pos.x_rot > 0) {
			item->pos.x_rot = 0;
		}
	}
}

short CreatureEffect(ITEM_INFO* item, BITE_INFO* bite, creature_effect_routine generate) {
	PHD_VECTOR bite_pos;
	PHD_VECTOR pos;
	PHD_VECTOR dir;
	bite_pos.x = bite->x;
	bite_pos.y = bite->y;
	bite_pos.z = bite->z;
	GetJointAbsPosition(item, &bite_pos, bite->mesh_num);
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetJointAbsPosition(item, &pos, bite->mesh_num);
	dir.x = 0;
	dir.y = 0;
	dir.z = 512;
	GetJointAbsPosition(item, &dir, bite->mesh_num);
	dir.x = dir.x - pos.x;
	dir.y = dir.y - pos.y;
	dir.z = dir.z - pos.z;
	Normalise(&dir);
	return generate(bite_pos.x, bite_pos.y, bite_pos.z, item->speed, item->pos.y_rot, dir, item->room_number);
}

short CreatureEffectT(ITEM_INFO* item, BITE_INFO* bite, short damage, short angle, creature_effect_routine generate) {
	PHD_VECTOR bite_pos;
	PHD_VECTOR pos;
	PHD_VECTOR dir;
	bite_pos.x = bite->x;
	bite_pos.y = bite->y;
	bite_pos.z = bite->z;
	GetJointAbsPosition(item, &bite_pos, bite->mesh_num);
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetJointAbsPosition(item, &pos, bite->mesh_num);
	dir.x = 0;
	dir.y = 0;
	dir.z = 512;
	GetJointAbsPosition(item, &dir, bite->mesh_num);
	dir.x = dir.x - pos.x;
	dir.y = dir.y - pos.y;
	dir.z = dir.z - pos.z;
	Normalise(&dir);
	return generate(bite_pos.x, bite_pos.y, bite_pos.z, damage, item->pos.y_rot, dir, item->room_number);
}

long CreatureVault(short item_number, short angle, long vault, long shift) {
	ITEM_INFO* item;
	long x, y, z, x_floor, z_floor;
	short room_number;

	item = GetItem(currentLevel, item_number);
	x = item->pos.pos.x >> 10;
	y = item->pos.pos.y;
	z = item->pos.pos.z >> 10;
	room_number = item->room_number;
	CreatureAnimation(item_number, angle, 0);

	if(item->floor > y + 1152) {
		vault = 0;
	} else if(item->floor > y + 896) {
		vault = -4;
	} else if(item->floor > y + 640 && item->object_number == VON_CROY) {
		vault = -3;
	} else if(item->floor > y + 384 && item->object_number == VON_CROY) {
		vault = -2;
	} else {
		if(item->pos.pos.y > y - 384) {
			return 0;
		}

		if(item->pos.pos.y > y - 640) {
			vault = 2;
		} else if(item->pos.pos.y > y - 896) {
			vault = 3;
		} else if(item->pos.pos.y > y - 1152) {
			vault = 4;
		}
	}

	x_floor = item->pos.pos.x >> 10;
	z_floor = item->pos.pos.z >> 10;

	if(z == z_floor) {
		if(x == x_floor) {
			return 0;
		}

		if(x >= x_floor) {
			item->pos.y_rot = -0x4000;
			item->pos.pos.x = shift + (x << 10);
		} else {
			item->pos.y_rot = 0x4000;
			item->pos.pos.x = (x_floor << 10) - shift;
		}
	} else if(x == x_floor) {
		if(z < z_floor) {
			item->pos.y_rot = 0;
			item->pos.pos.z = (z_floor << 10) - shift;
		} else {
			item->pos.y_rot = -0x8000;
			item->pos.pos.z = shift + (z << 10);
		}
	}

	item->floor = y;
	item->pos.pos.y = y;

	if(vault) {
		ItemNewRoom(item_number, room_number);
	}

	return vault;
}

void CreatureKill(ITEM_INFO* item, short kill_anim, short kill_state, short lara_anim) {
	item->anim_number = GetObjectInfo(currentLevel, item->object_number)->anim_index + kill_anim;
	item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
	item->current_anim_state = kill_state;
	lara_item->anim_number = lara_anim;
	lara_item->frame_number = GetAnim(currentLevel, lara_item->anim_number)->frame_base;
	lara_item->current_anim_state = 8;
	lara_item->goal_anim_state = 8;
	lara_item->pos.pos.x = item->pos.pos.x;
	lara_item->pos.pos.y = item->pos.pos.y;
	lara_item->pos.pos.z = item->pos.pos.z;
	lara_item->pos.x_rot = item->pos.x_rot;
	lara_item->pos.y_rot = item->pos.y_rot;
	lara_item->pos.z_rot = item->pos.z_rot;
	lara_item->fallspeed = 0;
	lara_item->gravity_status = 0;
	lara_item->speed = 0;

	if(lara_item->room_number != item->room_number) {
		ItemNewRoom(lara.item_number, item->room_number);
	}

	AnimateItem(lara_item);
	lara.gun_status = LG_HANDS_BUSY;
	lara.gun_type = WEAPON_NONE;
	lara.hit_direction = -1;
	lara.air = -1;
	camera.pos.room_number = lara_item->room_number;
	ForcedFixedCamera.pos.x = item->pos.pos.x + ((2048 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT);
	ForcedFixedCamera.pos.y = item->pos.pos.y - 1024;
	ForcedFixedCamera.pos.z = item->pos.pos.z + ((2048 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT);
	ForcedFixedCamera.room_number = item->room_number;
	UseForcedFixedCamera = 1;
}

void AlertAllGuards(short item_number) {
	ITEM_INFO* item;
	CREATURE_INFO* creature;

	item = GetItem(currentLevel, item_number);

	for(int i = 0; i < 5; i++) {
		creature = &baddie_slots[i];

		if(creature->item_num != NO_ITEM) {
			if(GetItem(currentLevel, creature->item_num)->object_number == item->object_number) {
				creature->alerted = 1;
			}
		}
	}
}

void AlertNearbyGuards(ITEM_INFO* item) {
	ITEM_INFO* target;
	CREATURE_INFO* creature;
	long dx, dy, dz, dist;

	for(int i = 0; i < 5; i++) {
		creature = &baddie_slots[i];

		if(creature->item_num == NO_ITEM) {
			continue;
		}

		target = GetItem(currentLevel, creature->item_num);

		if(target->room_number == item->room_number) {
			creature->alerted = 1;
			continue;
		}

		dx = (target->pos.pos.x - item->pos.pos.x) >> 6;
		dy = (target->pos.pos.y - item->pos.pos.y) >> 6;
		dz = (target->pos.pos.z - item->pos.pos.z) >> 6;
		dist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

		if(dist < 8000) {
			creature->alerted = 1;
		}
	}
}

short AIGuard(CREATURE_INFO* creature) {
	long rnd;

	if(GetItem(currentLevel, creature->item_num)->ai_bits & MODIFY) {
		return 0;
	}

	rnd = GetRandomControl();

	if(rnd < 256) {
		creature->head_left = 1;
		creature->head_right = 1;
	} else if(rnd < 384) {
		creature->head_left = 1;
		creature->head_right = 0;
	} else if(rnd < 512) {
		creature->head_left = 0;
		creature->head_right = 1;
	}

	if(creature->head_left && creature->head_right) {
		return 0;
	}

	if(creature->head_left) {
		return -0x4000;
	}

	if(creature->head_right) {
		return 0x4000;
	}

	return 0;
}

void FindAITargetObject(CREATURE_INFO* creature, short obj_num) {
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	AIOBJECT* aiObj;
	ROOM_INFO* r;
	short* zone;
	short zone_number, ai_zone;

	item = GetItem(currentLevel, creature->item_num);

	for(int i = 0; i < GetNumAIObjects(currentLevel); i++) {
		aiObj = GetAIObject(currentLevel, i);

		if(aiObj->object_number != obj_num || aiObj->trigger_flags != item->item_flags[3] || aiObj->room_number == 255) {
			continue;
		}

		zone = GetZone(currentLevel, creature->LOT.zone, flip_status);

		r = GetRoom(currentLevel, item->room_number);
		item->box_number = r->floor[((item->pos.pos.z - r->z) >> 10) + r->x_size * ((item->pos.pos.x - r->x) >> 10)].box;
		zone_number = zone[item->box_number];

		r = GetRoom(currentLevel, aiObj->room_number);
		aiObj->box_number = r->floor[((aiObj->z - r->z) >> 10) + r->x_size * ((aiObj->x - r->x) >> 10)].box;
		ai_zone = zone[aiObj->box_number];

		if(zone_number != ai_zone) {
			continue;
		}

		creature->enemy = &creature->ai_target;
		enemy = creature->enemy;
		enemy->object_number = aiObj->object_number;
		enemy->room_number = aiObj->room_number;
		enemy->pos.pos.x = aiObj->x;
		enemy->pos.pos.y = aiObj->y;
		enemy->pos.pos.z = aiObj->z;
		enemy->pos.y_rot = aiObj->y_rot;
		enemy->flags = aiObj->flags;
		enemy->trigger_flags = aiObj->trigger_flags;
		enemy->box_number = aiObj->box_number;

		if(!(enemy->flags & 0x20)) {
			enemy->pos.pos.x += 256 * phd_sin(enemy->pos.y_rot) >> W2V_SHIFT;
			enemy->pos.pos.z += 256 * phd_cos(enemy->pos.y_rot) >> W2V_SHIFT;
		}

		break;
	}
}

void GetAITarget(CREATURE_INFO* creature) {
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	short enemy_object;
	char ai_bits;

	enemy = creature->enemy;

	if(enemy) {
		enemy_object = enemy->object_number;
	} else {
		enemy_object = NO_ITEM;
	}

	item = GetItem(currentLevel, creature->item_num);
	ai_bits = (char)item->ai_bits;

	if(ai_bits & GUARD) {
		if(creature->alerted) {
			item->ai_bits &= ~GUARD;

			if(ai_bits & AMBUSH) {
				item->ai_bits |= MODIFY;
			}
		}
	} else if(ai_bits & PATROL1) {
		if(creature->alerted || creature->hurt_by_lara) {
			item->ai_bits &= ~PATROL1;

			if(ai_bits & AMBUSH) {
				item->ai_bits |= MODIFY;
			}
		} else if(!creature->patrol2) {
			if(enemy_object != AI_PATROL1) {
				FindAITargetObject(creature, AI_PATROL1);
			}
		} else if(creature->patrol2) {
			if(enemy_object != AI_PATROL2) {
				FindAITargetObject(creature, AI_PATROL2);
			}
		} else if(abs(enemy->pos.pos.x - item->pos.pos.x) < 640 && abs(enemy->pos.pos.y - item->pos.pos.y) < 640 && abs(enemy->pos.pos.z - item->pos.pos.z) < 640) {
			GetHeight(GetFloor(enemy->pos.pos.x, enemy->pos.pos.y, enemy->pos.pos.z, &enemy->room_number), enemy->pos.pos.x, enemy->pos.pos.y, enemy->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
			TestTriggers(trigger_index, 1, 0);
			creature->patrol2 = ~creature->patrol2;
		}
	} else if(ai_bits & AMBUSH) {
		if(enemy_object != AI_AMBUSH) {
			FindAITargetObject(creature, AI_AMBUSH);
		} else if(abs(enemy->pos.pos.x - item->pos.pos.x) < 640 && abs(enemy->pos.pos.y - item->pos.pos.y) < 640 && abs(enemy->pos.pos.z - item->pos.pos.z) < 640) {
			GetHeight(GetFloor(enemy->pos.pos.x, enemy->pos.pos.y, enemy->pos.pos.z, &enemy->room_number), enemy->pos.pos.x, enemy->pos.pos.y, enemy->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
			TestTriggers(trigger_index, 1, 0);
			creature->reached_goal = 1;
			creature->enemy = lara_item;
			item->ai_bits &= ~AMBUSH;

			if(item->ai_bits != MODIFY) {
				item->ai_bits |= GUARD;
				creature->alerted = 0;
			}
		}
	} else if(ai_bits & FOLLOW) {
		if(creature->hurt_by_lara) {
			creature->enemy = lara_item;
			creature->alerted = 1;
			item->ai_bits &= ~FOLLOW;
		} else if(item->hit_status) {
			item->ai_bits &= ~FOLLOW;
		} else if(enemy_object != AI_FOLLOW) {
			FindAITargetObject(creature, AI_FOLLOW);
		} else if(abs(enemy->pos.pos.x - item->pos.pos.x) < 640 && abs(enemy->pos.pos.y - item->pos.pos.y) < 640 && abs(enemy->pos.pos.z - item->pos.pos.z) < 640) {
			creature->reached_goal = 1;
			item->ai_bits &= ~FOLLOW;
		}
	}
}

short SameZone(CREATURE_INFO* creature, ITEM_INFO* target_item) {
	ITEM_INFO* item;
	ROOM_INFO* r;
	short* zone;

	zone = GetZone(currentLevel, creature->LOT.zone, flip_status);
	item = GetItem(currentLevel, creature->item_num);

	r = GetRoom(currentLevel, item->room_number);
	item->box_number = r->floor[((item->pos.pos.z - r->z) >> 10) + r->x_size * ((item->pos.pos.x - r->x) >> 10)].box;

	r = GetRoom(currentLevel, target_item->room_number);
	target_item->box_number = r->floor[((target_item->pos.pos.z - r->z) >> 10) + r->x_size * ((target_item->pos.pos.x - r->x) >> 10)].box;

	return zone[item->box_number] == zone[target_item->box_number];
}

void CreatureYRot(PHD_3DPOS* srcpos, short angle, short angadd) {
	if(angle > angadd) {
		srcpos->y_rot += angadd;
	} else if(angle < -angadd) {
		srcpos->y_rot -= angadd;
	} else {
		srcpos->y_rot += angle;
	}
}

long MoveCreature3DPos(PHD_3DPOS* srcpos, PHD_3DPOS* destpos, long velocity, short angdif, long angadd) {
	long x, y, z, dist;

	x = destpos->pos.x - srcpos->pos.x;
	y = destpos->pos.y - srcpos->pos.y;
	z = destpos->pos.z - srcpos->pos.z;
	dist = phd_sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));

	if(velocity < dist) {
		srcpos->pos.x += velocity * x / dist;
		srcpos->pos.y += velocity * y / dist;
		srcpos->pos.z += velocity * z / dist;
	} else {
		srcpos->pos.x = destpos->pos.x;
		srcpos->pos.y = destpos->pos.y;
		srcpos->pos.z = destpos->pos.z;
	}

	if(angdif > angadd) {
		srcpos->y_rot += (short)angadd;
	} else if(angdif < -angadd) {
		srcpos->y_rot -= (short)angadd;
	} else {
		srcpos->y_rot = destpos->y_rot;
	}

	return srcpos->pos.x == destpos->pos.x && srcpos->pos.y == destpos->pos.y && srcpos->pos.z == destpos->pos.z && srcpos->y_rot == destpos->y_rot;
}
