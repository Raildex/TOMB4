
#include "game/sentrygun.h"
#include "game/effect2.h"
#include "specific/function_stubs.h"
#include "game/levelinfo.h"
#include "game/sphere.h"
#include "game/box.h"
#include "game/tomb4fx.h"
#include "game/lot.h"
#include "game/items.h"
#include "game/deltapak.h"
#include "game/objects.h"
#include "game/sound.h"
#include "game/people.h"
#include "game/newinv.h"
#include "game/control.h"
#include "game/sparks.h"
#include "game/iteminfo.h"
#include "game/itemstatus.h"
#include "game/itemflags.h"
#include "game/biteinfo.h"
#include "game/creatureinfo.h"
#include "game/aiinfo.h"
#include "global/math_tbls.h"

static BITE_INFO AGOffsets = { 0, 0, 0, 8 };

void TriggerAutogunFlamethrower(ITEM_INFO* item) {
	SPARKS* sptr;
	PHD_VECTOR pos;
	PHD_VECTOR vel;
	long v;

	for(int i = 0; i < 3; i++) {
		sptr = GetFreeSpark();
		sptr->On = 1;
		sptr->sR = (GetRandomControl() & 0x1F) + 48;
		sptr->sG = 48;
		sptr->sB = 255;
		sptr->dR = (GetRandomControl() & 0x3F) + 192;
		sptr->dG = (GetRandomControl() & 0x3F) + 128;
		sptr->dB = 32;
		sptr->ColFadeSpeed = 12;
		sptr->FadeToBlack = 8;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 0x1F) + 16;
		sptr->sLife = sptr->Life;

		pos.x = -140;
		pos.y = -30;
		pos.z = -4;
		GetJointAbsPosition(item, &pos, 7);
		sptr->x = (GetRandomControl() & 0x1F) + pos.x - 16;
		sptr->y = (GetRandomControl() & 0x1F) + pos.y - 16;
		sptr->z = (GetRandomControl() & 0x1F) + pos.z - 16;

		vel.x = -280;
		vel.y = -30;
		vel.z = -4;
		GetJointAbsPosition(item, &vel, 7);
		v = (GetRandomControl() & 0x3F) + 192;
		sptr->Xvel = (short)(v * (vel.x - pos.x) / 10);
		sptr->Yvel = (short)(v * (vel.y - pos.y) / 10);
		sptr->Zvel = (short)(v * (vel.z - pos.z) / 10);

		sptr->Friction = 85;
		sptr->MaxYvel = 0;
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->Flags = 538;

		if(GlobalCounter & 1) {
			v = 255;
			sptr->Flags = 539;
		}

		sptr->Scalar = 3;
		v *= (GetRandomControl() & 7) + 60;
		sptr->dSize = (unsigned char)(v >> 8);
		sptr->Size = (unsigned char)(v >> 12);
		sptr->sSize = sptr->Size;
	}
}

void InitialiseAutogun(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel,item_number);
	InitialiseCreature(item_number);
	item->item_flags[0] = 0;
	item->item_flags[1] = 768;
	item->item_flags[2] = 0;
}

void AutogunControl(short item_number) {
	ITEM_INFO* item;
	CREATURE_INFO* autogun;
	AI_INFO info;
	PHD_VECTOR pos;
	long ang;
	short ahead;

	ang = 0;
	item = GetItem(currentLevel,item_number);

	if(!CreatureActive(item_number))
		return;

	autogun = (CREATURE_INFO*)item->data;

	if(!autogun)
		return;

	if(!(item->mesh_bits & 0x40)) {
		ExplodingDeath2(item_number, -1, 257);
		DisableBaddieAI(item_number);
		KillItem(item_number);
		item->status = ITEM_DEACTIVATED;
		item->flags |= IFL_INVISIBLE;
		untrigger_item_in_room(item->room_number, SMOKE_EMITTER_BLACK);
		TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos - 768, item->pos.z_pos, 3, -2, 0, item->room_number);

		for(int i = 0; i < 2; i++)
			TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos - 768, item->pos.z_pos, 3, -1, 0, item->room_number);

		SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800000 | SFX_SETPITCH);
		SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
	} else {
		if(item->item_flags[0]) {
			pos.x = AGOffsets.x;
			pos.y = AGOffsets.y;
			pos.z = AGOffsets.z;
			GetJointAbsPosition(item, &pos, AGOffsets.mesh_num);
			TriggerDynamic(pos.x, pos.y, pos.z, (item->item_flags[0] << 2) + 12, 24, 16, 4);
			item->item_flags[0]--;
		}

		if(item->item_flags[0] & 1)
			item->mesh_bits |= 0x100;
		else
			item->mesh_bits &= ~0x100;

		if(!item->trigger_flags) {
			item->pos.y_pos -= 512;
			CreatureAIInfo(item, &info);
			item->pos.y_pos += 512;
			ahead = info.angle - autogun->joint_rotation[0];

			if(ahead > -0x4000 && ahead < 0x4000)
				info.ahead = 1;
			else
				info.ahead = 0;

			if(Targetable(item, &info) && info.distance < 0x5100000) {
				if(!have_i_got_object(PUZZLE_ITEM5) && !item->item_flags[0]) {
					if(info.distance > 0x400000) {
						item->item_flags[0] = 2;
						ShotLara(item, &info, &AGOffsets, autogun->joint_rotation[0], 5);
						SoundEffect(SFX_AUTOGUNS, &item->pos, SFX_DEFAULT);
						item->item_flags[2] += 256;

						if(item->item_flags[2] > 6144)
							item->item_flags[2] = 6144;
					} else {
						TriggerAutogunFlamethrower(item);
						ang = (4 * rcossin_tbl[(2048 * (GlobalCounter & 0x1F)) >> 3]) >> 2;
					}
				}

				ang += info.angle - autogun->joint_rotation[0];

				if(ang > 1820)
					ang = 1820;
				else if(ang < -1820)
					ang = -1820;

				autogun->joint_rotation[0] += (short)ang;
				CreatureJoint(item, 1, -info.x_angle);
			}

			item->item_flags[2] -= 32;

			if(item->item_flags[2] < 0)
				item->item_flags[2] = 0;

			autogun->joint_rotation[2] += item->item_flags[1];
			autogun->joint_rotation[3] += item->item_flags[2];

			if(autogun->joint_rotation[2] > 0x4000 || autogun->joint_rotation[2] < -0x4000)
				item->item_flags[1] = -item->item_flags[1];
		} else {
			CreatureJoint(item, 1, 0x2000);
			CreatureJoint(item, 2, (GetRandomControl() & 0x3FFF) - 0x2000);
			CreatureJoint(item, 0, (GetRandomControl() & 0x7FF) - 1024);
		}
	}
}
