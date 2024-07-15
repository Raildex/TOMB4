
#include "game/laraflar.h"
#include "flareinfo.h"
#include "game/animstruct.h"
#include "game/collide.h"
#include "game/control.h"
#include "game/delstuff.h"
#include "game/draw.h"
#include "game/effect2.h"
#include "game/gameflow.h"
#include "game/gfleveloptions.h"
#include "game/heighttypes.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/itemstatus.h"
#include "game/lara.h"
#include "game/lara_states.h"
#include "game/larafire.h"
#include "game/laragunstatus.h"
#include "game/larainfo.h"
#include "game/laramesh.h"
#include "game/levelinfo.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/roomflags.h"
#include "game/roominfo.h"
#include "game/sound.h"
#include "game/tomb4fx.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/output.h"
#include <stdlib.h>


void DrawFlareInAir(ITEM_INFO* item) {
	short bounds[6];

	GetBoundsAccurate(item, bounds);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.pos.x, item->pos.pos.y - bounds[3], item->pos.pos.z);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_PutPolygons_train(GetMesh(currentLevel, GetObjectInfo(currentLevel, FLARE_ITEM)->mesh_index), 0);
	phd_PopMatrix();

	if(gfLevelFlags & GF_MIRROR) {
		if(item->room_number == gfMirrorRoom) {
			phd_PushMatrix();
			phd_TranslateAbs(item->pos.pos.x, item->pos.pos.y, 2 * gfMirrorZPlane - item->pos.pos.z);
			phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
			phd_PutPolygons_train(GetMesh(currentLevel, GetObjectInfo(currentLevel, FLARE_ITEM)->mesh_index), 0);
			phd_PopMatrix();
		}
	}
}

void draw_flare_meshes() {
	lara.mesh_ptrs[LM_LHAND] = GetMesh(currentLevel, GetObjectInfo(currentLevel, FLARE_ANIM)->mesh_index + LM_LHAND * 2);
}

void undraw_flare_meshes() {
	lara.mesh_ptrs[LM_LHAND] = GetMesh(currentLevel, GetObjectInfo(currentLevel, LARA)->mesh_index + LM_LHAND * 2);
}

long DoFlareLight(PHD_VECTOR* pos, long flare_age) {
	long x, y, z, r, g, b, rnd, falloff, ret;

	if(flare_age >= 900 || !flare_age) {
		return 0;
	}

	rnd = GetRandomControl();
	x = pos->x + ((rnd & 0xF) << 3);
	y = pos->y + (rnd >> 1 & 120) - 256;
	z = pos->z + (rnd >> 5 & 120);
	ret = 1;

	if(flare_age < 4) {
		falloff = (rnd & 3) + 4 + flare_age * 4;

		if(falloff > 16) {
			falloff -= (rnd >> 12) & 3;
		}

		r = (rnd >> 4 & 0x1F) + 8 * flare_age + 32;
		g = (rnd & 0x1F) + 16 * (flare_age + 10);
		b = 16 * flare_age + ((rnd >> 8) & 0x1F);
	} else if(flare_age < 16) {
		falloff = (rnd & 1) + flare_age + 2;
		r = ((rnd >> 4) & 0x1F) + 4 * flare_age + 64;
		g = (rnd & 0x3F) + 4 * flare_age + 128;
		b = ((rnd >> 8) & 0x1F) + 4 * flare_age + 16;
	} else if(flare_age < 810) {
		falloff = 16;
		r = (rnd >> 4 & 0x1F) + 128;
		g = (rnd & 0x3F) + 192;
		b = ((rnd >> 8) & 0x3F) + (((rnd >> 6) & 0x7F) >> 2);
	} else if(flare_age < 876) {
		if(rnd > 0x2000) {
			falloff = 16;
			r = (rnd & 0x1F) + 128;
			g = (rnd & 0x3F) + 192;
			b = ((rnd >> 8) & 0x3F) + (((rnd >> 6) & 0x7F) >> 2);
		} else {
			r = (GetRandomControl() & 0x3F) + 64;
			g = (GetRandomControl() & 0x3F) + 192;
			b = GetRandomControl() & 0x7F;
			falloff = (GetRandomControl() & 6) + 8;
			ret = 0;
		}
	} else {
		falloff = 16 - ((flare_age - 876) >> 1);
		r = (GetRandomControl() & 0x3F) + 64;
		g = (GetRandomControl() & 0x3F) + 192;
		b = GetRandomControl() & 0x1F;
		ret = rnd & 1;
	}

	TriggerDynamic(x, y, z, falloff, r, g, b);
	return ret;
}

void DoFlareInHand(long flare_age) {
	PHD_VECTOR pos;

	pos.x = 11;
	pos.y = 32;
	pos.z = 41;
	GetLaraJointPos(&pos, 14);
	lara.left_arm.flash_gun = (short)DoFlareLight(&pos, flare_age);

	if(gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom) {
		pos.z = 2 * gfMirrorZPlane - pos.z;
		DoFlareLight(&pos, flare_age);
	}

	if(lara.flare_age < 900) {
		lara.flare_age++;
	} else if(lara.gun_status == LG_NO_ARMS) {
		lara.gun_status = LG_UNDRAW_GUNS;
	}
}

void CreateFlare(short object, long thrown) {
	ITEM_INFO* itemlist[6] = { 0 };
	MESH_INFO* meshlist[6] = { 0 };
	ITEM_INFO* flare;
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	PHD_VECTOR pos;
	long collided;
	short flare_item, room_number;

	flare_item = CreateItem();

	if(flare_item != NO_ITEM) {
		collided = 0;
		flare = GetItem(currentLevel, flare_item);
		flare->object_number = object;
		flare->room_number = lara_item->room_number;

		pos.x = -16;
		pos.y = 32;
		pos.z = 42;
		GetLaraJointPos(&pos, 14);
		flare->pos.pos.x = pos.x;
		flare->pos.pos.y = pos.y;
		flare->pos.pos.z = pos.z;

		room_number = lara_item->room_number;
		floor = GetFloor(pos.x, pos.y, pos.z, &room_number);

		if(GetCollidedObjects(flare, 0, 1, itemlist, 5, meshlist, 5, 0) || pos.y > GetHeight(floor, pos.x, pos.y, pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject)) {
			collided = 1;
			flare->pos.y_rot = lara_item->pos.y_rot - 0x8000;
			flare->pos.pos.x = lara_item->pos.pos.x + (80 * phd_sin(flare->pos.y_rot) >> W2V_SHIFT);
			flare->pos.pos.z = lara_item->pos.pos.z + (80 * phd_cos(flare->pos.y_rot) >> W2V_SHIFT);
			flare->room_number = lara_item->room_number;
		} else {
			if(thrown) {
				flare->pos.y_rot = lara_item->pos.y_rot;
			} else {
				flare->pos.y_rot = lara_item->pos.y_rot - 0x2000;
			}

			flare->room_number = room_number;
		}

		InitialiseItem(flare_item);
		flare->pos.x_rot = 0;
		flare->pos.z_rot = 0;
		flare->shade = -1;

		if(thrown) {
			flare->speed = lara_item->speed + 50;
			flare->fallspeed = lara_item->fallspeed - 50;
		} else {
			flare->speed = lara_item->speed + 10;
			flare->fallspeed = lara_item->fallspeed + 50;
		}

		if(collided) {
			flare->speed >>= 1;
		}

		if(object == FLARE_ITEM) {
			FLARE_INFO* info = (FLARE_INFO*)calloc(1, sizeof(FLARE_INFO));
			DoFlareLight((PHD_VECTOR*)&flare->pos, lara.flare_age);
			info->age = (lara.flare_age & 0x7FFF);
			flare->data = info;
		} else {
			flare->item_flags[3] = lara.LitTorch;
		}

		AddActiveItem(flare_item);
		flare->status = ITEM_ACTIVE;
	}
}

void set_flare_arm(long frame) {
	short anim_base;

	anim_base = GetObjectInfo(currentLevel, FLARE_ANIM)->anim_index;

	if(frame >= 1) {
		if(frame < 33) {
			anim_base += 1;
		} else if(frame < 72) {
			anim_base += 2;
		} else if(frame < 95) {
			anim_base += 3;
		} else {
			anim_base += 4;
		}
	}

	lara.left_arm.anim_number = anim_base;
	lara.left_arm.frame_base = GetAnim(currentLevel, anim_base)->frame_ptr;
}

void ready_flare() {
	lara.gun_status = LG_NO_ARMS;
	lara.left_arm.x_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.z_rot = 0;
	lara.right_arm.x_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
	lara.target_item = NO_ITEM;
}

void draw_flare() {
	short ani;

	if(lara_item->current_anim_state == AS_FLAREPICKUP || lara_item->current_anim_state == AS_PICKUP) {
		DoFlareInHand(lara.flare_age);
		lara.flare_control_left = 0;
		ani = 93;
	} else {
		lara.flare_control_left = 1;
		ani = lara.left_arm.frame_number + 1;

		if(ani < 33 || ani > 94) {
			ani = 33;
		} else if(ani == 46) {
			draw_flare_meshes();
		} else if(ani >= 72 && ani <= 93) {
			if(ani == 72) {
				if(GetRoom(currentLevel, lara_item->room_number)->flags & ROOM_UNDERWATER) {
					SoundEffect(SFX_OBJ_GEM_SMASH, &lara_item->pos, SFX_WATER);
				} else {
					SoundEffect(SFX_OBJ_GEM_SMASH, &lara_item->pos, SFX_DEFAULT);
				}

				lara.flare_age = 1;
			}

			DoFlareInHand(lara.flare_age);
		} else if(ani == 94) {
			ready_flare();
			ani = 0;
			DoFlareInHand(lara.flare_age);
		}
	}

	lara.left_arm.frame_number = ani;
	set_flare_arm(ani);
}

void undraw_flare() {
	short ani, ani2;

	ani = lara.left_arm.frame_number;
	ani2 = lara.flare_frame;
	lara.flare_control_left = 1;

	if(lara_item->goal_anim_state == AS_STOP && lara.vehicle == NO_ITEM) {
		if(lara_item->anim_number == ANIM_BREATH) {
			lara_item->anim_number = ANIM_THROWFLARE;
			ani2 = ani + GetAnim(currentLevel, ANIM_THROWFLARE)->frame_base;
			lara.flare_frame = ani2;
			lara_item->frame_number = ani2;
		}

		if(lara_item->anim_number == ANIM_THROWFLARE) {
			lara.flare_control_left = 0;

			if(ani2 >= GetAnim(currentLevel, ANIM_THROWFLARE)->frame_base + 31) {
				lara.request_gun_type = lara.last_gun_type;
				lara.gun_type = lara.last_gun_type;
				lara.gun_status = LG_NO_ARMS;
				InitialiseNewWeapon();
				lara.target_item = NO_ITEM;
				lara.right_arm.lock = 0;
				lara.left_arm.lock = 0;
				lara_item->anim_number = ANIM_STOP;
				lara_item->frame_number = GetAnim(currentLevel, ANIM_STOP)->frame_base;
				lara_item->current_anim_state = AS_STOP;
				lara_item->goal_anim_state = AS_STOP;
				lara.flare_frame = GetAnim(currentLevel, ANIM_STOP)->frame_base;
				return;
			}

			ani2++;
			lara.flare_frame = ani2;
		}
	} else if(lara_item->current_anim_state == AS_STOP && lara.vehicle == NO_ITEM) {
		lara_item->anim_number = ANIM_STOP;
		lara_item->frame_number = GetAnim(currentLevel, ANIM_STOP)->frame_base;
	}

	if(ani >= 33 && ani < 72) {
		ani = 1;
	}

	if(!ani) {
		ani = 1;
	} else if(ani >= 72 && ani < 95) {
		ani++;

		if(ani == 94) {
			ani = 1;
		}
	} else if(ani >= 1 && ani < 33) {
		ani++;

		if(ani == 21) {
			CreateFlare(FLARE_ITEM, 1);
			undraw_flare_meshes();
			lara.flare_age = 0;
		} else if(ani == 33) {
			ani = 0;
			lara.gun_type = lara.last_gun_type;
			lara.request_gun_type = lara.last_gun_type;
			lara.gun_status = LG_NO_ARMS;
			;
			InitialiseNewWeapon();
			lara.target_item = NO_ITEM;
			lara.left_arm.lock = 0;
			lara.right_arm.lock = 0;
			lara.flare_control_left = 0;
			lara.flare_frame = 0;
		}
	} else if(ani >= 95 && ani < 110) {
		ani++;

		if(ani == 110) {
			ani = 1;
		}
	}

	if(ani >= 1 && ani < 21) {
		DoFlareInHand(lara.flare_age);
	}

	lara.left_arm.frame_number = ani;
	set_flare_arm(lara.left_arm.frame_number);
}

void FlareControl(short item_number) {
	ITEM_INFO* flare;
	long x, y, z, xv, yv, zv, flare_age;

	flare = GetItem(currentLevel, item_number);
	FLARE_INFO* data = flare->data;
	if(flare->fallspeed) {
		flare->pos.x_rot += 546;
		flare->pos.z_rot += 910;
	} else {
		flare->pos.x_rot = 0;
		flare->pos.z_rot = 0;
	}

	x = flare->pos.pos.x;
	y = flare->pos.pos.y;
	z = flare->pos.pos.z;
	xv = flare->speed * phd_sin(flare->pos.y_rot) >> W2V_SHIFT;
	zv = flare->speed * phd_cos(flare->pos.y_rot) >> W2V_SHIFT;
	flare->pos.pos.x += xv;
	flare->pos.pos.z += zv;

	if(GetRoom(currentLevel, flare->room_number)->flags & ROOM_UNDERWATER) {
		flare->fallspeed += (5 - flare->fallspeed) >> 1;
		flare->speed += (5 - flare->speed) >> 1;
	} else {
		flare->fallspeed += 6;
	}

	yv = flare->fallspeed;
	flare->pos.pos.y += yv;
	DoProperDetection(item_number, x, y, z, xv, yv, zv);
	flare_age = data->age;

	if(flare_age >= 900) {
		if(!flare->fallspeed && !flare->speed) {
			KillItem(item_number);
			return;
		}
	} else {
		flare_age++;
	}

	if(DoFlareLight((PHD_VECTOR*)&flare->pos, flare_age)) {
		if(gfLevelFlags & GF_MIRROR && flare->room_number == gfMirrorRoom) {
			flare->pos.pos.z = 2 * gfMirrorZPlane - flare->pos.pos.z;
			DoFlareLight((PHD_VECTOR*)&flare->pos, flare_age);
			flare->pos.pos.z = 2 * gfMirrorZPlane - flare->pos.pos.z;
		}

		// flare_age |= 0x8000;
	}

	data->age = flare_age;
}
