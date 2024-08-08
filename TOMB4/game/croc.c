
#include "game/croc.h"
#include "game/aibits.h"
#include "game/aiinfo.h"
#include "game/animstruct.h"
#include "game/biteinfo.h"
#include "game/box.h"
#include "game/control.h"
#include "game/creatureinfo.h"
#include "game/draw.h"
#include "game/effect2.h"
#include "game/effects.h"
#include "game/fxinfo.h"
#include "game/heighttypes.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/lara.h"
#include "game/larainfo.h"
#include "game/levelinfo.h"
#include "game/locuststruct.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/people.h"
#include "game/phd3dpos.h"
#include "game/phdvector.h"
#include "game/roomflags.h"
#include "game/roominfo.h"
#include "game/sound.h"
#include "game/sparks.h"
#include "game/sphere.h"
#include "game/tomb4fx.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/output.h"
#include <stdlib.h>


LOCUST_STRUCT Locusts[64];

static BITE_INFO croc_bite = { 0, -100, 500, 9 };
static long next_locust = 0;

void InitialiseCroc(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);
	InitialiseCreature(item_number);

	if(GetRoom(currentLevel, item->room_number)->flags & ROOM_UNDERWATER) {
		item->anim_number = GetObjectInfo(currentLevel, CROCODILE)->anim_index + 12;
		item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
		item->current_anim_state = 8;
		item->goal_anim_state = 8;
	} else {
		item->anim_number = GetObjectInfo(currentLevel, CROCODILE)->anim_index;
		item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
		item->current_anim_state = 1;
		item->goal_anim_state = 1;
	}
}

void CrocControl(short item_number) {
	ITEM_INFO* item;
	CREATURE_INFO* croc;
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	AI_INFO info;
	long s, c, x, z, h, h2;
	short room_number, angle, rot, roll;

	if(!CreatureActive(item_number)) {
		return;
	}

	item = GetItem(currentLevel, item_number);
	croc = (CREATURE_INFO*)item->data;
	angle = 0;
	rot = 0;
	s = (1024 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	c = (1024 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
	x = item->pos.pos.x + s;
	z = item->pos.pos.z + c;
	room_number = item->room_number;
	floor = GetFloor(x, item->pos.pos.y, z, &room_number);
	h = GetHeight(floor, x, item->pos.pos.y, z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

	if(abs(item->pos.pos.y - h) > 512) {
		h = item->pos.pos.y;
	}

	x = item->pos.pos.x - s;
	z = item->pos.pos.z - c;
	room_number = item->room_number;
	floor = GetFloor(x, item->pos.pos.y, z, &room_number);
	h2 = GetHeight(floor, x, item->pos.pos.y, z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

	if(abs(item->pos.pos.y - h2) > 512) {
		h2 = item->pos.pos.y;
	}

	roll = (short)phd_atan(2048, h2 - h);

	if(item->hit_points <= 0) {
		item->hit_points = 0;

		if(item->current_anim_state != 7 && item->current_anim_state != 10) {
			if(GetRoom(currentLevel, item->room_number)->flags & ROOM_UNDERWATER) {
				item->anim_number = GetObjectInfo(currentLevel, CROCODILE)->anim_index + 16;
				item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
				item->current_anim_state = 10;
				item->goal_anim_state = 10;
				item->hit_points = -16384;
			} else {
				item->anim_number = GetObjectInfo(currentLevel, CROCODILE)->anim_index + 11;
				item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
				item->current_anim_state = 7;
				item->goal_anim_state = 7;
			}
		}

		if(GetRoom(currentLevel, item->room_number)->flags & ROOM_UNDERWATER) {
			CreatureFloat(item_number);
		}
	} else {
		if(item->ai_bits) {
			GetAITarget(croc);
		} else if(croc->hurt_by_lara) {
			croc->enemy = lara_item;
		}

		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, croc->maximum_turn);

		if(item->hit_status || info.distance < 0x240000 || (TargetVisible(item, &info) && info.distance < 0x1900000)) {
			if(!croc->alerted) {
				croc->alerted = 1;
			}

			AlertAllGuards(item_number);
		}

		rot = angle << 2;

		switch(item->current_anim_state) {
		case 1:
			croc->maximum_turn = 0;

			if(item->ai_bits & GUARD) {
				rot = item->item_flags[0];
				item->goal_anim_state = 1;
				item->item_flags[0] += item->item_flags[1];

				if(!(GetRandomControl() & 0x1F)) {
					if(GetRandomControl() & 1) {
						item->item_flags[1] = 0;
					} else {
						if(GetRandomControl() & 1) {
							item->item_flags[1] = 12;
						} else {
							item->item_flags[1] = -12;
						}
					}
				}

				if(item->item_flags[0] > 1024) {
					item->item_flags[0] = 1024;
				} else if(item->item_flags[0] < -1024) {
					item->item_flags[0] = -1024;
				}
			} else if(info.bite && info.distance < 0x90000) {
				item->goal_anim_state = 5;
			} else if(info.ahead && info.distance < 0x100000) {
				item->goal_anim_state = 3;
			} else {
				item->goal_anim_state = 2;
			}

			break;

		case 2:
			croc->maximum_turn = 546;

			if(item->required_anim_state) {
				item->goal_anim_state = item->required_anim_state;
			} else if(info.bite && info.distance < 0x90000) {
				item->goal_anim_state = 1;
			} else if(info.ahead && info.distance < 0x100000) {
				item->goal_anim_state = 3;
			}

			break;

		case 3:
			croc->maximum_turn = 546;
			croc->LOT.step = 256;
			croc->LOT.drop = -256;

			if(item->required_anim_state) {
				item->goal_anim_state = item->required_anim_state;
			} else if(info.bite && info.distance < 0x90000) {
				item->goal_anim_state = 1;
			} else if(!info.ahead || info.distance > 0x240000) {
				item->goal_anim_state = 2;
			}

			break;

		case 5:

			if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base) {
				item->required_anim_state = 0;
			}

			if(info.bite && item->touch_bits & 0x300) {
				if(!item->required_anim_state) {
					CreatureEffectT(item, &croc_bite, 10, -1, DoBloodSplat);
					lara_item->hit_points -= 120;
					lara_item->hit_status = 1;
					item->required_anim_state = 1;
				}
			} else {
				item->goal_anim_state = 1;
			}

			break;

		case 8:
			croc->maximum_turn = 546;
			croc->LOT.step = 20480;
			croc->LOT.drop = -20480;

			if(item->required_anim_state) {
				item->goal_anim_state = item->required_anim_state;
			} else if(info.bite && item->touch_bits & 0x300) {
				item->goal_anim_state = 9;
			}

			break;

		case 9:

			if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base) {
				item->required_anim_state = 0;
			}

			if(info.bite && item->touch_bits & 0x300) {
				if(!item->required_anim_state) {
					CreatureEffectT(item, &croc_bite, 10, -1, DoBloodSplat);
					lara_item->hit_points -= 120;
					lara_item->hit_status = 1;
					item->required_anim_state = 8;
				}
			} else {
				item->goal_anim_state = 8;
			}

			break;
		default:
			break;
		}
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, rot);
	CreatureJoint(item, 1, rot);
	CreatureJoint(item, 2, -rot);
	CreatureJoint(item, 3, -rot);

	if(item->current_anim_state < 8) {
		if(abs(roll - item->pos.x_rot) < 256) {
			item->pos.x_rot = roll;
		} else if(roll > item->pos.x_rot) {
			item->pos.x_rot += 256;
		} else if(roll < item->pos.x_rot) {
			item->pos.x_rot -= 256;
		}
	}

	CreatureAnimation(item_number, angle, 0);

	if(item->current_anim_state == 8) {
		s = (1024 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	} else {
		s = (512 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	}

	c = (1024 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
	x = item->pos.pos.x + s;
	z = item->pos.pos.z + c;
	room_number = item->room_number;
	GetFloor(x, item->pos.pos.y, z, &room_number);

	if(GetRoom(currentLevel, item->room_number)->flags & ROOM_UNDERWATER) {
		if(GetRoom(currentLevel, room_number)->flags & 1) {
			if(item->current_anim_state == 2) {
				item->required_anim_state = 3;
				item->goal_anim_state = 3;
			} else if(item->current_anim_state == 3) {
				item->required_anim_state = 8;
				item->goal_anim_state = 8;
			} else if(item->anim_number != GetObjectInfo(currentLevel, CROCODILE)->anim_index + 17) {
				croc->LOT.step = 20480;
				croc->LOT.drop = -20480;
				croc->LOT.fly = 16;
				CreatureUnderwater(item, 256);
			}
		} else {
			item->required_anim_state = 3;
			item->goal_anim_state = 3;
			croc->LOT.step = 256;
			croc->LOT.drop = -256;
			croc->LOT.fly = 0;
			CreatureUnderwater(item, 0);
		}
	} else {
		croc->LOT.fly = 0;
	}
}

long GetFreeLocust() {
	LOCUST_STRUCT* fx;

	fx = &Locusts[next_locust];

	for(int free = next_locust, i = 0; i < 64; i++) {
		if(fx->On) {
			if(free == 63) {
				fx = Locusts;
				free = 0;
			} else {
				free++;
				fx++;
			}
		} else {
			next_locust = (free + 1) & 0x3F;
			return free;
		}
	}

	return NO_ITEM;
}

void TriggerLocust(ITEM_INFO* item) {
	LOCUST_STRUCT* fx;
	PHD_VECTOR vec;
	PHD_VECTOR vec2;
	long fx_number;
	short angles[2];

	fx_number = GetFreeLocust();

	if(fx_number == NO_ITEM) {
		return;
	}

	fx = &Locusts[fx_number];

	if(item->object_number == FISH) {
		vec.x = item->pos.pos.x;
		vec.y = item->pos.pos.y;
		vec.z = item->pos.pos.z;
		*(long*)angles = item->pos.y_rot + 0x8000;
	} else {
		vec2.x = 0;
		vec2.y = -96;
		vec2.z = 144;
		GetJointAbsPosition(item, &vec2, 9);
		vec.x = 0;
		vec.y = -128;
		vec.z = 288;
		GetJointAbsPosition(item, &vec, 9);
		phd_GetVectorAngles(vec.x - vec2.x, vec.y - vec2.y, vec.z - vec2.z, angles);
	}

	fx->room_number = item->room_number;
	fx->pos.pos.x = vec.x;
	fx->pos.pos.y = vec.y;
	fx->pos.pos.z = vec.z;
	fx->pos.x_rot = (GetRandomControl() & 0x3FF) + angles[1] - 512;
	fx->pos.y_rot = (GetRandomControl() & 0x7FF) + angles[0] - 1024;
	fx->On = 1;
	fx->flags = 0;
	fx->speed = (GetRandomControl() & 0x1F) + 16;
	fx->LaraTarget = GetRandomControl() & 0x1FF;
	fx->Counter = 20 * ((GetRandomControl() & 7) + 15);
}

void InitialiseLocustEmitter(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(!item->pos.y_rot) {
		item->pos.pos.z += 512;
	} else if(item->pos.y_rot == 16384) {
		item->pos.pos.x += 512;
	} else if(item->pos.y_rot == -32768) {
		item->pos.pos.z -= 512;
	} else if(item->pos.y_rot == -16384) {
		item->pos.pos.x -= 512;
	}
}

void ControlLocustEmitter(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(!TriggerActive(item)) {
		return;
	}

	if(item->trigger_flags) {
		TriggerLocust(item);
		item->trigger_flags--;
	} else {
		KillItem(item_number);
	}
}

void DrawLocusts() {
	LOCUST_STRUCT* fx;
	short** meshpp;

	for(int i = 0; i < 64; i++) {
		fx = &Locusts[i];

		if(fx->On) {
			meshpp = GetMeshPointer(currentLevel, GetObjectInfo(currentLevel, AHMET_MIP)->mesh_index + 2 * (-GlobalCounter & 3));
			phd_PushMatrix();
			phd_TranslateAbs(fx->pos.pos.x, fx->pos.pos.y, fx->pos.pos.z);
			phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
			phd_PutPolygons_train(*meshpp, 0);
			phd_PopMatrix();
		}
	}
}

void UpdateLocusts() {
	LOCUST_STRUCT* fx;
	short* lb;
	short bounds[6];
	long speed, ox, oy, oz, closestdist, closestnum;
	short angles[2];
	short max_turn;

	GetBoundsAccurate(lara_item, bounds);
	bounds[0] += lara_item->pos.pos.x;
	bounds[1] += lara_item->pos.pos.x;
	bounds[2] += lara_item->pos.pos.y;
	bounds[3] += lara_item->pos.pos.y;
	bounds[4] += lara_item->pos.pos.z;
	bounds[5] += lara_item->pos.pos.z;
	closestdist = 0xFFFFFFF;
	closestnum = -1;

	for(int i = 0; i < 64; i++) {
		fx = &Locusts[i];

		if(fx->On) {
			if((lara.burn || lara_item->hit_points <= 0) && fx->Counter > 90 && !(GetRandomControl() & 7)) {
				fx->Counter = 90;
			}

			fx->Counter--;

			if(!fx->Counter) {
				fx->On = 0;
				continue;
			}

			if(!(GetRandomControl() & 7)) {
				fx->LaraTarget = (GetRandomControl() % 640) + 128;
				fx->XTarget = (GetRandomControl() & 0x7F) - 64;
				fx->ZTarget = (GetRandomControl() & 0x7F) - 64;
			}

			phd_GetVectorAngles(
				lara_item->pos.pos.x + (fx->XTarget << 3) - fx->pos.pos.x,
				lara_item->pos.pos.y - fx->LaraTarget - fx->pos.pos.y,
				lara_item->pos.pos.z + (fx->ZTarget << 3) - fx->pos.pos.z,
				angles);

			ox = SQUARE(lara_item->pos.pos.x - fx->pos.pos.x);
			oz = SQUARE(lara_item->pos.pos.z - fx->pos.pos.z);

			if(ox + oz < closestdist) {
				closestdist = ox + oz;
				closestnum = i;
			}

			ox = phd_sqrt(ox + oz) >> 3;

			if(ox > 128) {
				ox = 128;
			} else if(ox < 48) {
				ox = 48;
			}

			if(fx->speed < ox) {
				fx->speed++;
			} else if(fx->speed > ox) {
				fx->speed--;
			}

			if(fx->Counter > 90) {
				max_turn = fx->speed << 7;
				oy = (unsigned short)angles[0] - (unsigned short)fx->pos.y_rot;

				if(abs(oy) > 32768) {
					oy = (unsigned short)fx->pos.y_rot - (unsigned short)angles[0];
				}

				ox = (unsigned short)angles[1] - (unsigned short)fx->pos.x_rot;

				if(abs(ox) > 32768) {
					ox = (unsigned short)fx->pos.x_rot - (unsigned short)angles[0];
				}

				ox >>= 3;
				oy >>= 3;

				if(oy > max_turn) {
					oy = max_turn;
				} else if(oy < -max_turn) {
					oy = -max_turn;
				}
				if(ox > max_turn) {
					ox = max_turn;
				} else if(ox < -max_turn) {
					ox = -max_turn;
				}

				fx->pos.y_rot += (short)oy;
				fx->pos.x_rot += (short)ox;
			}

			ox = fx->pos.pos.x;
			oy = fx->pos.pos.y;
			oz = fx->pos.pos.z;
			speed = fx->speed * phd_cos(fx->pos.x_rot) >> W2V_SHIFT;
			fx->pos.pos.x += speed * phd_sin(fx->pos.y_rot) >> W2V_SHIFT;
			fx->pos.pos.y += fx->speed * phd_sin(-fx->pos.x_rot) >> W2V_SHIFT;
			fx->pos.pos.z += speed * phd_cos(fx->pos.y_rot) >> W2V_SHIFT;

			if(!(i & 1)) {
				if(fx->pos.pos.x > bounds[0] && fx->pos.pos.x < bounds[1] && fx->pos.pos.y > bounds[2] && fx->pos.pos.y < bounds[3] && fx->pos.pos.z > bounds[4] && fx->pos.pos.z < bounds[5]) {
					TriggerBlood(fx->pos.pos.x, fx->pos.pos.y, fx->pos.pos.z, GetRandomControl() << 1, 2);

					if(lara_item->hit_points > 0) {
						lara_item->hit_points -= 3;
					}
				}
			}
		}
	}

	if(closestnum != -1) {
		fx = &Locusts[closestnum];
		SoundEffect(SFX_LOCUSTS_LOOP, (PHD_VECTOR*)&fx->pos, fx->room_number, SFX_DEFAULT);
	}
}

void TriggerCrocgodMissile(PHD_3DPOS* pos, short room_number, short num) {
	FX_INFO* fx;
	short fx_number;

	fx_number = CreateEffect(room_number);

	if(fx_number != NO_ITEM) {
		fx = GetEffect(currentLevel, fx_number);
		fx->pos.pos.x = pos->pos.x;
		fx->pos.pos.y = pos->pos.y - (GetRandomControl() & 0x3F) - 32;
		fx->pos.pos.z = pos->pos.z;
		fx->pos.x_rot = pos->x_rot;
		fx->pos.y_rot = pos->y_rot;
		fx->pos.z_rot = 0;
		fx->room_number = room_number;
		fx->counter = 15 + (num << 4);
		fx->flag1 = 6;
		fx->object_number = BUBBLES;
		fx->speed = (GetRandomControl() & 0x1F) + 96;
		fx->frame_number = GetObjectInfo(currentLevel, BUBBLES)->mesh_index + 10;
	}
}

void TriggerCrocgodMissileFlame(short fx_number, long xv, long yv, long zv) {
	FX_INFO* fx;
	SPARKS* sptr;
	long dx, dz;

	fx = GetEffect(currentLevel, fx_number);
	dx = lara_item->pos.pos.x - fx->pos.pos.x;
	dz = lara_item->pos.pos.z - fx->pos.pos.z;

	if(dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000) {
		return;
	}

	sptr = GetFreeSpark();
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) + 128;
	sptr->sG = sptr->sR >> 1;
	sptr->sB = 0;
	sptr->dR = (GetRandomControl() & 0x3F) + 128;
	sptr->dG = sptr->dR >> 1;
	sptr->dB = 0;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->Life = (GetRandomControl() & 7) + 32;
	sptr->sLife = sptr->Life;
	sptr->x = fx->pos.pos.x + (GetRandomControl() & 0xF) - 8;
	sptr->y = fx->pos.pos.y;
	sptr->z = fx->pos.pos.z + (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (short)xv;
	sptr->Yvel = (short)yv;
	sptr->Zvel = (short)zv;
	sptr->Friction = 34;
	sptr->Flags = 538;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if(GetRandomControl() & 1) {
		sptr->RotAdd = -32 - (GetRandomControl() & 0x1F);
	} else {
		sptr->RotAdd = (GetRandomControl() & 0x1F) + 32;
	}

	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->FxObj = (unsigned char)fx_number;
	sptr->Scalar = 2;
	sptr->Size = (GetRandomControl() & 0xF) + 128;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 2;
}

void InitialiseCrocgod(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);
	InitialiseCreature(item_number);
	item->anim_number = GetObjectInfo(currentLevel, MUTANT)->anim_index;
	item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;
}

void CrocgodControl(short item_number) {
	ITEM_INFO* item;
	CREATURE_INFO* crocgod;
	AI_INFO info;
	PHD_3DPOS mPos;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	short angles[2];
	short angle, torso, neck, frame;

	if(!CreatureActive(item_number)) {
		return;
	}

	angle = 0;
	torso = 0;
	neck = 0;
	item = GetItem(currentLevel, item_number);
	crocgod = (CREATURE_INFO*)item->data;

	if(item->hit_points <= 0) {
		item->hit_points = 0;
	} else {
		if(item->ai_bits) {
			GetAITarget(crocgod);
		} else if(crocgod->hurt_by_lara) {
			crocgod->enemy = lara_item;
		}

		item->pos.pos.y -= 768;
		CreatureAIInfo(item, &info);
		item->pos.pos.y += 768;

		if(crocgod->enemy != lara_item) {
			phd_atan(lara_item->pos.pos.z - item->pos.pos.z, lara_item->pos.pos.x - item->pos.pos.x);
		}

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		crocgod->maximum_turn = 0;
		angle = CreatureTurn(item, 0);

		if(item->item_flags[2] == 999) {
			torso = info.angle;
			neck = info.x_angle;
		}

		switch(item->current_anim_state) {
		case 2:

			if(item->item_flags[2] < 600) {
				item->goal_anim_state = 4;
				item->item_flags[2]++;
			} else {
				item->item_flags[2] = 999;

				if(info.distance < 0x1900000) {
					item->goal_anim_state = 5;
				} else if(info.distance < 0x3840000) {
					item->goal_anim_state = 3;
				} else if(info.distance < 0x7900000) {
					item->goal_anim_state = 4;
				}
			}

			break;

		case 3:
			frame = item->frame_number - GetAnim(currentLevel, item->anim_number)->frame_base;

			if(frame >= 94 && frame <= 96) {
				pos.x = 0;
				pos.y = -96;
				pos.z = 144;
				GetJointAbsPosition(item, &pos, 9);
				pos2.x = 0;
				pos2.y = -128;
				pos2.z = 288;
				GetJointAbsPosition(item, &pos2, 9);
				mPos.pos.z = pos2.z;
				mPos.pos.y = pos2.y;
				mPos.pos.x = pos2.x;
				phd_GetVectorAngles(pos2.x - pos.x, pos2.y - pos.y, pos2.z - pos.z, angles);
				mPos.y_rot = angles[0];
				mPos.x_rot = angles[1];

				if(frame == 94) {
					TriggerCrocgodMissile(&mPos, item->room_number, 0);
				} else {
					if(frame == 95) {
						mPos.y_rot = angles[0] - 2048;
					} else {
						mPos.y_rot = angles[0] + 2048;
					}

					TriggerCrocgodMissile(&mPos, item->room_number, 1);
				}
			}

			break;

		case 4:

			if(item->item_flags[2] < 600) {
				item->item_flags[2]++;
			}

			if(item->item_flags[2] == 999) {
				frame = item->frame_number - GetAnim(currentLevel, item->anim_number)->frame_base;

				if(frame >= 60 && frame <= 120) {
					TriggerLocust(item);
				}
			}

			break;

		case 5:
			frame = item->frame_number - GetAnim(currentLevel, item->anim_number)->frame_base;

			if(frame == 45 || frame == 60 || frame == 75) {
				pos.x = 0;
				pos.y = -96;
				pos.z = 144;
				GetJointAbsPosition(item, &pos, 9);
				pos2.x = 0;
				pos2.y = -128;
				pos2.z = 288;
				GetJointAbsPosition(item, &pos2, 9);
				mPos.pos.z = pos2.z;
				mPos.pos.y = pos2.y;
				mPos.pos.x = pos2.x;
				phd_GetVectorAngles(pos2.x - pos.x, pos2.y - pos.y, pos2.z - pos.z, angles);
				mPos.y_rot = angles[0];
				mPos.x_rot = angles[1];

				if(frame == 60) {
					TriggerCrocgodMissile(&mPos, item->room_number, 0);
				} else {
					TriggerCrocgodMissile(&mPos, item->room_number, 1);
				}
			}

			break;
		default:
			break;
		}
	}

	CreatureJoint(item, 0, torso);
	CreatureJoint(item, 1, neck);
	CreatureJoint(item, 2, torso);
	CreatureJoint(item, 3, neck);
	CreatureAnimation(item_number, angle, 0);
}
