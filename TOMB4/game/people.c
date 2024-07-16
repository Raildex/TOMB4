
#include "game/people.h"
#include "box.h"
#include "game/aiinfo.h"
#include "game/biteinfo.h"
#include "game/box.h"
#include "game/control.h"
#include "game/creatureinfo.h"
#include "game/draw.h"
#include "game/effects.h"
#include "game/gamevector.h"
#include "game/lara.h"
#include "game/larainfo.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/sound.h"
#include "game/sphere.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"


enum no_distance {
	NO_DISTANCE = 0x40000000
};

short GunShot(long x, long y, long z, short speed, short yrot,PHD_VECTOR dir, short room_number) {
	return -1;
}

short GunHit(long x, long y, long z, short speed, short yrot,PHD_VECTOR dir, short room_number) {
	PHD_VECTOR pos;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetJointAbsPosition(lara_item, &pos, (25 * GetRandomControl()) / 0x7FFF);
	DoBloodSplat(pos.x, pos.y, pos.z, (GetRandomControl() & 3) + 3, lara_item->pos.y_rot, dir, lara_item->room_number);
	SoundEffect(SFX_LARA_INJURY, &lara_item->pos, SFX_DEFAULT);
	return GunShot(x, y, z, speed, yrot, dir, room_number);
}

short GunMiss(long x, long y, long z, short speed, short yrot, PHD_VECTOR dir, short room_number) {
	GAME_VECTOR pos;

	pos.pos.x = lara_item->pos.pos.x + ((GetRandomControl() - 0x4000) << 9) / 0x7FFF;
	pos.pos.y = lara_item->floor;
	pos.pos.z = lara_item->pos.pos.z + ((GetRandomControl() - 0x4000) << 9) / 0x7FFF;
	pos.room_number = lara_item->room_number;
	Richochet(&pos);
	return GunShot(x, y, z, speed, yrot, dir, room_number);
}

long TargetVisible(ITEM_INFO* item, AI_INFO* info) {
	ITEM_INFO* enemy;
	CREATURE_INFO* creature;
	GAME_VECTOR start;
	GAME_VECTOR target;
	short* bounds;

	creature = (CREATURE_INFO*)item->data;
	enemy = creature->enemy;

	if(!enemy || enemy->hit_points <= 0 || !enemy->data || info->angle - creature->joint_rotation[2] <= -0x4000 || info->angle - creature->joint_rotation[2] >= 0x4000 || info->distance >= NO_DISTANCE) {
		return 0;
	}

	bounds = GetBestFrame(enemy);

	start.pos.x = item->pos.pos.x;
	start.pos.y = item->pos.pos.y - 768;
	start.pos.z = item->pos.pos.z;
	start.room_number = item->room_number;

	target.pos.x = enemy->pos.pos.x;
	target.pos.y = enemy->pos.pos.y + ((bounds[3] + 3 * bounds[2]) >> 2);
	target.pos.z = enemy->pos.pos.z;
	return LOS(&start, &target);
}

long Targetable(ITEM_INFO* item, AI_INFO* info) {
	ITEM_INFO* enemy;
	CREATURE_INFO* creature;
	GAME_VECTOR start;
	GAME_VECTOR target;
	short* bounds;

	creature = (CREATURE_INFO*)item->data;
	enemy = creature->enemy;

	if(!enemy || enemy->hit_points <= 0 || !enemy->data || !info->ahead || (info->distance >= NO_DISTANCE && item->object_number != SETHA)) {
		return 0;
	}

	bounds = GetBestFrame(item);
	start.pos.x = item->pos.pos.x;
	start.pos.y = item->pos.pos.y + ((bounds[3] + 3 * bounds[2]) >> 2);
	start.pos.z = item->pos.pos.z;
	start.room_number = item->room_number;

	bounds = GetBestFrame(enemy);
	target.pos.x = enemy->pos.pos.x;
	target.pos.y = enemy->pos.pos.y + ((bounds[3] + 3 * bounds[2]) >> 2);
	target.pos.z = enemy->pos.pos.z;
	return LOS(&start, &target);
}

long ShotLara(ITEM_INFO* item, AI_INFO* info, BITE_INFO* gun, short extra_rotation, long damage) {
	ITEM_INFO* enemy;
	CREATURE_INFO* creature;
	PHD_VECTOR pos;
	PHD_VECTOR dir;
	long hit, targetable, random, distance;

	creature = (CREATURE_INFO*)item->data;
	enemy = creature->enemy;

	if(info->distance <= NO_DISTANCE && Targetable(item, info)) {
		distance = phd_sin(info->enemy_facing) * enemy->speed * NO_DISTANCE / 300 >> W2V_SHIFT;
		distance = info->distance + SQUARE(distance);

		if(distance <= NO_DISTANCE) {
			random = (NO_DISTANCE - info->distance) / 3276 + 0x2000;
			hit = (GetRandomControl() < random);
		} else {
			hit = 0;
		}

		targetable = 1;
	} else {
		hit = 0;
		targetable = 0;
	}

	if(damage) {
		if(enemy == lara_item) {
			if(hit) {
				CreatureEffect(item, gun, GunHit);
				lara_item->hit_points -= (short)damage;
				lara_item->hit_status = 1;
			} else if(targetable) {
				CreatureEffect(item, gun, GunMiss);
			}
		} else {
			CreatureEffect(item, gun, GunShot);

			if(hit) {
				enemy->hit_points -= (short)(damage / 10);
				enemy->hit_status = 1;
				random = GetRandomControl() & 0xF;

				if(random > 14) {
					random = 0;
				}

				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				GetJointAbsPosition(enemy, &pos, random);
				dir.x = 0;
				dir.y = 0;
				dir.z = 1;
				DoBloodSplat(pos.x, pos.y, pos.z, (GetRandomControl() & 3) + 4, enemy->pos.y_rot, dir, enemy->room_number);
			}
		}
	}

	return targetable;
}
