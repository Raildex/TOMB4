#include "game/effects.h"
#include "game/attachediteminfo.h"
#include "game/camera.h"
#include "game/control.h"
#include "game/deltapak.h"
#include "game/draw.h"
#include "game/effect2.h"
#include "game/footprnt.h"
#include "game/gameflow.h"
#include "game/hair.h"
#include "game/itemflags.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/itemstatus.h"
#include "game/lara.h"
#include "game/lara_states.h"
#include "game/laragunstatus.h"
#include "game/laramesh.h"
#include "game/levelinfo.h"
#include "game/lot.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/objectvector.h"
#include "game/pickup.h"
#include "game/roomflags.h"
#include "game/roominfo.h"
#include "game/sampleinfo.h"
#include "game/savegame.h"
#include "game/savegameinfo.h"
#include "game/scarab.h"
#include "game/sound.h"
#include "game/soundslot.h"
#include "game/splashsetup.h"
#include "game/tomb4fx.h"
#include "global/types.h"
#include "lara.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/function_table.h"
#include "specific/polyinsert.h"
#include "specific/sound.h"


long GlobalFogOff = 0;

long FogTableColor[28] = {
	0,
	RGBONLY(245, 200, 60),
	RGBONLY(120, 196, 112),
	RGBONLY(202, 204, 230),
	RGBONLY(128, 64, 0),
	RGBONLY(64, 64, 64),
	RGBONLY(243, 232, 236),
	RGBONLY(0, 64, 192),
	RGBONLY(0, 128, 0),
	RGBONLY(150, 172, 157),
	RGBONLY(128, 128, 128),
	RGBONLY(204, 163, 123),
	RGBONLY(177, 162, 140),
	RGBONLY(0, 223, 191),
	RGBONLY(111, 255, 223),
	RGBONLY(244, 216, 152),
	RGBONLY(248, 192, 60),
	RGBONLY(252, 0, 0),
	RGBONLY(198, 95, 87),
	RGBONLY(226, 151, 118),
	RGBONLY(248, 235, 206),
	RGBONLY(0, 30, 16),
	RGBONLY(250, 222, 167),
	RGBONLY(218, 175, 117),
	RGBONLY(225, 191, 78),
	RGBONLY(77, 140, 141),
	RGBONLY(4, 181, 154),
	RGBONLY(255, 174, 0)
};

void SetFog(ITEM_INFO* item) {
	GlobalFogOff = 0;

	if(IsVolumetric()) {
		if(TriggerTimer == 100) {
			GlobalFogOff = 1;
		} else {
			gfFog.r = CLRR(FogTableColor[TriggerTimer]);
			gfFog.g = CLRG(FogTableColor[TriggerTimer]);
			gfFog.b = CLRB(FogTableColor[TriggerTimer]);
			savegame.fog_colour.r = gfFog.r;
			savegame.fog_colour.g = gfFog.g;
			savegame.fog_colour.b = gfFog.b;
			SetFogColor(gfFog.r, gfFog.g, gfFog.b);
		}
	}

	flipeffect = -1;
}

void finish_level_effect(ITEM_INFO* item) {
	gfLevelComplete = gfCurrentLevel + 1;
}

void turn180_effect(ITEM_INFO* item) {
	item->pos.y_rot += 0x8000;
	item->pos.x_rot = -item->pos.x_rot;
}

void floor_shake_effect(ITEM_INFO* item) {
	long dx, dy, dz, dist;

	dx = item->pos.pos.x - camera.pos.pos.x;
	dy = item->pos.pos.y - camera.pos.pos.y;
	dz = item->pos.pos.z - camera.pos.pos.z;

	if(abs(dx) < 0x4000 && abs(dy) < 0x4000 && abs(dz) < 0x4000) {
		dist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);
		camera.bounce = -66 * (0x100000 - dist / 256) / 0x100000;
	}
}

void SoundFlipEffect(ITEM_INFO* item) {
	SoundEffect(TriggerTimer, 0, item->room_number, SFX_DEFAULT);
	flipeffect = -1;
}

void RubbleFX(ITEM_INFO* item) {
	ITEM_INFO* eq;

	eq = find_a_fucking_item(EARTHQUAKE);

	if(eq) {
		AddActiveItem(GetItemNum(currentLevel, eq));
		eq->status = ITEM_ACTIVE;
		eq->flags |= IFL_CODEBITS;
	} else {
		camera.bounce = -150;
	}

	flipeffect = -1;
}

void PoseidonSFX(ITEM_INFO* item) {
	SoundEffect(SFX_WATER_FLUSHES, 0, item->room_number, SFX_DEFAULT);
	flipeffect = -1;
}

void ActivateCamera(ITEM_INFO* item) {
	KeyTriggerActive = 2;
}

void ActivateKey(ITEM_INFO* item) {
	KeyTriggerActive = 1;
}

void SwapCrowbar(ITEM_INFO* item) {
	short* tmp;

	tmp = GetMesh(currentLevel, GetObjectInfo(currentLevel, LARA)->mesh_index + 2 * LM_RHAND);

	if(lara.mesh_ptrs[LM_RHAND] == tmp) {
		lara.mesh_ptrs[LM_RHAND] = GetMesh(currentLevel, GetObjectInfo(currentLevel, CROWBAR_ANIM)->mesh_index + (2 * LM_RHAND));
	} else {
		lara.mesh_ptrs[LM_RHAND] = tmp;
	}
}

void ExplosionFX(ITEM_INFO* item) {
	SoundEffect(SFX_EXPLOSION1, 0, item->room_number, SFX_DEFAULT);
	camera.bounce = -75;
	flipeffect = -1;
}

void LaraLocation(ITEM_INFO* item) {
	lara.location = TriggerTimer;

	if(TriggerTimer > lara.highest_location) {
		lara.highest_location = TriggerTimer;
	}

	flipeffect = -1;
}

void LaraLocationPad(ITEM_INFO* item) {
	flipeffect = -1;
	lara.locationPad = TriggerTimer;
	lara.location = TriggerTimer;
}

void GhostTrap(ITEM_INFO* item) {
	ITEM_INFO* wraith;
	short nex;

	nex = next_item_active;

	if(next_item_active != NO_ITEM) {
		while(nex != NO_ITEM) {
			wraith = GetItem(currentLevel, nex);

			if(wraith->object_number == WRAITH3 && !wraith->hit_points) {
				wraith->hit_points = GetItemNum(currentLevel, item);
				break;
			}

			nex = wraith->next_active;
		}
	}

	flipeffect = -1;
}

void KillActiveBaddies(ITEM_INFO* item) {
	ITEM_INFO* target_item;
	short item_num;

	for(item_num = next_item_active; item_num != NO_ITEM; item_num = target_item->next_active) {
		target_item = GetItem(currentLevel, item_num);

		if(GetObjectInfo(currentLevel, target_item->object_number)->intelligent) {
			target_item->status = ITEM_INVISIBLE;

			if(item != ((void*)0xABCDEF)) {
				RemoveActiveItem(item_num);
				DisableBaddieAI(item_num);
				target_item->flags |= IFL_INVISIBLE;
			}
		}
	}

	flipeffect = -1;
}

void lara_hands_free(ITEM_INFO* item) {
	lara.gun_status = LG_NO_ARMS;
}

void draw_right_gun(ITEM_INFO* item) {
	short* tmp;

	tmp = lara.mesh_ptrs[LM_RTHIGH];
	lara.mesh_ptrs[LM_RTHIGH] = GetMesh(currentLevel, GetObjectInfo(currentLevel, PISTOLS_ANIM)->mesh_index + LM_RTHIGH * 2);
	*GetMeshPointer(currentLevel, GetObjectInfo(currentLevel, PISTOLS_ANIM)->mesh_index + LM_RTHIGH * 2) = tmp;

	tmp = lara.mesh_ptrs[LM_RHAND];
	lara.mesh_ptrs[LM_RHAND] = GetMesh(currentLevel, GetObjectInfo(currentLevel, PISTOLS_ANIM)->mesh_index + LM_RHAND * 2);
	*GetMeshPointer(currentLevel, GetObjectInfo(currentLevel, PISTOLS_ANIM)->mesh_index + LM_RHAND * 2) = tmp;
}

void draw_left_gun(ITEM_INFO* item) {
	short* tmp;

	tmp = lara.mesh_ptrs[LM_LTHIGH];
	lara.mesh_ptrs[LM_LTHIGH] = GetMesh(currentLevel, GetObjectInfo(currentLevel, PISTOLS_ANIM)->mesh_index + LM_LTHIGH * 2);
	*GetMeshPointer(currentLevel, GetObjectInfo(currentLevel, PISTOLS_ANIM)->mesh_index + LM_LTHIGH * 2) = tmp;

	tmp = lara.mesh_ptrs[LM_LHAND];
	lara.mesh_ptrs[LM_LHAND] = GetMesh(currentLevel, GetObjectInfo(currentLevel, PISTOLS_ANIM)->mesh_index + LM_LHAND * 2);
	*GetMeshPointer(currentLevel, GetObjectInfo(currentLevel, PISTOLS_ANIM)->mesh_index + LM_LHAND * 2) = tmp;
}

void shoot_right_gun(ITEM_INFO* item) {
	lara.right_arm.flash_gun = 3;
}

void shoot_left_gun(ITEM_INFO* item) {
	lara.left_arm.flash_gun = 3;
}

void swap_meshes_with_meshswap1(ITEM_INFO* item) {
	OBJECT_INFO* obj;
	short* tmp;

	obj = GetObjectInfo(currentLevel, item->object_number);

	for(int i = 0; i < obj->nmeshes; i++) {
		tmp = GetMesh(currentLevel, obj->mesh_index + i * 2);
		*GetMeshPointer(currentLevel, obj->mesh_index + i) = GetMesh(currentLevel, GetObjectInfo(currentLevel, MESHSWAP1)->mesh_index + i * 2);
		*GetMeshPointer(currentLevel, GetObjectInfo(currentLevel, MESHSWAP1)->mesh_index + i * 2) = tmp;
	}
}

void swap_meshes_with_meshswap2(ITEM_INFO* item) {
	OBJECT_INFO* obj;
	short* tmp;

	obj = GetObjectInfo(currentLevel, item->object_number);

	for(int i = 0; i < obj->nmeshes; i++) {
		tmp = GetMesh(currentLevel, obj->mesh_index + i * 2);
		*GetMeshPointer(currentLevel, obj->mesh_index + i) = GetMesh(currentLevel, GetObjectInfo(currentLevel, MESHSWAP2)->mesh_index + i * 2);
		*GetMeshPointer(currentLevel, GetObjectInfo(currentLevel, MESHSWAP2)->mesh_index + i * 2) = tmp;
	}
}

void swap_meshes_with_meshswap3(ITEM_INFO* item) {
	OBJECT_INFO* obj;
	short* tmp;

	obj = GetObjectInfo(currentLevel, item->object_number);

	for(int i = 0; i < obj->nmeshes; i++) {
		tmp = GetMesh(currentLevel, obj->mesh_index + i * 2);
		*GetMeshPointer(currentLevel, obj->mesh_index + i) = GetMesh(currentLevel, GetObjectInfo(currentLevel, MESHSWAP3)->mesh_index + i * 2);

		if(item == lara_item) {
			lara.mesh_ptrs[i] = GetMesh(currentLevel, GetObjectInfo(currentLevel, MESHSWAP3)->mesh_index + i * 2);
		}

		*GetMeshPointer(currentLevel, GetObjectInfo(currentLevel, MESHSWAP3)->mesh_index + i * 2) = tmp;
	}
}

void invisibility_on(ITEM_INFO* item) {
	item->status = ITEM_INVISIBLE;
}

void invisibility_off(ITEM_INFO* item) {
	item->status = ITEM_ACTIVE;
}

void reset_hair(ITEM_INFO* item) {
	InitialiseHair();
}

void ClearScarabsPatch(ITEM_INFO* item) {
	ClearScarabs();
}

void MeshSwapToPour(ITEM_INFO* item) {
	lara.mesh_ptrs[LM_LHAND] = GetMesh(currentLevel, GetObjectInfo(currentLevel, item->item_flags[2])->mesh_index + LM_LHAND * 2);
}

void MeshSwapFromPour(ITEM_INFO* item) {
	lara.mesh_ptrs[LM_LHAND] = GetMesh(currentLevel, GetObjectInfo(currentLevel, LARA_SKIN)->mesh_index + LM_LHAND * 2);
}

void void_effect(ITEM_INFO* item) {
}

void WaterFall(short item_number) {
	ITEM_INFO* item;
	long dx, dy, dz;

	item = GetItem(currentLevel, item_number);
	dx = item->pos.pos.x - lara_item->pos.pos.x;
	dy = item->pos.pos.y - lara_item->pos.pos.y;
	dz = item->pos.pos.z - lara_item->pos.pos.z;

	if(dx >= -0x4000 && dx <= 0x4000 && dz >= -0x4000 && dz <= 0x4000 && dy >= -0x4000 && dy <= 0x4000) {
		// empty func call here

		if(!(wibble & 0xC)) {
			dx = (136 * phd_sin(item->pos.y_rot)) >> 12;
			dz = (136 * phd_cos(item->pos.y_rot)) >> 12;
			TriggerWaterfallMist(item->pos.pos.x + dx, item->pos.pos.y, item->pos.pos.z + dz, item->pos.y_rot >> 4);
		}

		SoundEffect(SFX_WATERFALL_LOOP, (PHD_VECTOR*)&item->pos, item->room_number, 0);
	}
}

void WadeSplash(ITEM_INFO* item, long water, long depth) {
	short* bounds;
	short room_number;

	room_number = item->room_number;
	GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);

	if(!(GetRoom(currentLevel, room_number)->flags & ROOM_UNDERWATER)) {
		return;
	}

	bounds = GetBestFrame(item);

	if(item->pos.pos.y + bounds[2] > water || item->pos.pos.y + bounds[3] < water) {
		return;
	}

	if(item->fallspeed > 0 && depth < 474 && !SplashCount) {
		splash_setup.pos.x = item->pos.pos.x;
		splash_setup.pos.y = water;
		splash_setup.pos.z = item->pos.pos.z;
		splash_setup.InnerRad = 16;
		splash_setup.InnerSize = 12;
		splash_setup.InnerRadVel = 160;
		splash_setup.InnerYVel = -72 * item->fallspeed;
		splash_setup.pad1 = 24;
		splash_setup.MiddleRad = 24;
		splash_setup.MiddleSize = 224;
		splash_setup.MiddleRadVel = -36 * item->fallspeed;
		splash_setup.MiddleYVel = 32;
		splash_setup.pad2 = 32;
		splash_setup.OuterRad = 272;
		SetupSplash(&splash_setup);
		SplashCount = 16;
	} else if(!(wibble & 0xF) && (!(GetRandomControl() & 0xF) || item->current_anim_state != AS_STOP)) {
		if(item->current_anim_state == AS_STOP) {
			SetupRipple(item->pos.pos.x, water, item->pos.pos.z, (GetRandomControl() & 0xF) + 112, 16);
		} else {
			SetupRipple(item->pos.pos.x, water, item->pos.pos.z, (GetRandomControl() & 0xF) + 112, 18);
		}
	}
}

void Splash(ITEM_INFO* item) {
	short room_number;

	room_number = item->room_number;
	GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);

	if(GetRoom(currentLevel, room_number)->flags & ROOM_UNDERWATER) {
		splash_setup.pos.x = item->pos.pos.x;
		splash_setup.pos.y = GetWaterHeight(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, room_number);
		splash_setup.pos.z = item->pos.pos.z;
		splash_setup.InnerRad = 32;
		splash_setup.InnerSize = 8;
		splash_setup.InnerRadVel = 320;
		splash_setup.InnerYVel = -40 * item->fallspeed;
		splash_setup.pad1 = 48;
		splash_setup.MiddleRad = 32;
		splash_setup.MiddleSize = 480;
		splash_setup.MiddleRadVel = -20 * item->fallspeed;
		splash_setup.MiddleYVel = 32;
		splash_setup.pad2 = 128;
		splash_setup.OuterRad = 544;
		SetupSplash(&splash_setup);
	}
}

short DoBloodSplat(long x, long y, long z, short speed, short ang, PHD_VECTOR dir, short room_number) {
	long wh;
	if(GetRoom(currentLevel, room_number)->flags & ROOM_UNDERWATER) {
		wh = GetWaterHeight(x, y, z, room_number);
		if(y < wh + 512) {
			y = wh + 512;
		}
		TriggerUnderwaterBlood(x, y, z, speed);
	} else {
		TriggerBlood(x, y, z, ang >> 4, speed);
	}

	return -1;
}

void DoLotsOfBlood(long x, long y, long z, short speed, short ang, short room_number, long num) {
	long bx, by, bz;
	PHD_VECTOR dir;
	for(; num > 0; num--) {
		bx = x - (GetRandomControl() << 9) / 0x8000 + 256;
		by = y - (GetRandomControl() << 9) / 0x8000 + 256;
		bz = z - (GetRandomControl() << 9) / 0x8000 + 256;
		dir.x = bx;
		dir.y = by;
		dir.z = bz;
		DoBloodSplat(bx, by, bz, speed, ang, dir, room_number);
	}
}

void Richochet(GAME_VECTOR* pos) {
	TriggerRicochetSpark(pos, mGetAngle(pos->pos.z, pos->pos.x, lara_item->pos.pos.z, lara_item->pos.pos.x) >> 4, 3, 0);
	SoundEffect(SFX_LARA_RICOCHET, (PHD_VECTOR*)pos,pos->room_number, SFX_DEFAULT);
}

void SoundEffects() {
	OBJECT_VECTOR* sfx;
	SoundSlot* slot;

	for(int i = 0; i < GetNumSoundEffects(currentLevel); i++) {
		sfx = GetSoundEffect(currentLevel, i);

		if(flip_status) {
			if(sfx->flags & 0x40) {
				SoundEffect(sfx->data, (PHD_VECTOR*)sfx,-1, 0);
			}
		} else if(sfx->flags & 0x80) {
			SoundEffect(sfx->data, (PHD_VECTOR*)sfx,-1, 0);
		}
	}

	if(flipeffect != -1) {
		effect_routines[flipeffect](0);
	}

	if(!sound_active) {
		return;
	}

	for(int i = 0; i < 32; i++) {
		slot = &LaSlot[i];

		if(slot->nSampleInfo < 0) {
			continue;
		}

		if((GetSampleInfo(currentLevel, slot->nSampleInfo)->flags & 3) != 3) {
			if(!S_SoundSampleIsPlaying(soundImpl, i)) {
				slot->nSampleInfo = -1;
			} else {
				GetPanVolume(slot);
				S_SoundSetPanAndVolume(soundImpl, i, (short)slot->nPan, (unsigned short)slot->nVolume);
			}
		} else {
			if(!slot->nVolume) {
				S_SoundStopSample(soundImpl, i);
				slot->nSampleInfo = -1;
			} else {
				S_SoundSetPanAndVolume(soundImpl, i, (short)slot->nPan, (unsigned short)slot->nVolume);
				S_SoundSetPitch(soundImpl, i, slot->nPitch);
				slot->nVolume = 0;
			}
		}
	}
}

long ItemNearLara(PHD_3DPOS* pos, long rad) {
	short bounds[6];
	long dx, dy, dz;

	dx = pos->pos.x - lara_item->pos.pos.x;
	dy = pos->pos.y - lara_item->pos.pos.y;
	dz = pos->pos.z - lara_item->pos.pos.z;

	if(dx >= -rad && dx <= rad && dz >= -rad && dz <= rad && dy >= -3072 && dy <= 3072 && SQUARE(dx) + SQUARE(dz) <= SQUARE(rad)) {
		GetBoundsAccurate(lara_item, bounds);

		if(dy >= bounds[2] && dy <= bounds[3] + 100) {
			return 1;
		}
	}

	return 0;
}

void AttachKeyPuzzleObject(ITEM_INFO* item) {
	if(item->object_number == LARA) {
		LARA_INFO* l = (LARA_INFO*)item->data;
		short keyPuzzleItemNo = l->GeneralPtr;
		ITEM_INFO* keyPuzzleItem = GetItem(currentLevel, keyPuzzleItemNo);
		enum object_types obj = keyPuzzleItem->object_number;
		if(obj >= PUZZLE_HOLE1 && obj <= PUZZLE_HOLE12) {
			obj = obj - PUZZLE_HOLE1 + PUZZLE_ITEM1;
		} else if(obj >= KEY_HOLE1 && obj <= KEY_HOLE12) {
			obj = obj - KEY_HOLE1 + KEY_ITEM1;
		}
		short attachedItem = CreateItem();
		if(attachedItem == NO_ITEM) {
			return;
		}
		ITEM_INFO* attached = GetItem(currentLevel, attachedItem);
		attached->object_number = ATTACHED_OBJECT;
		attached->room_number = item->room_number;
		InitialiseItem(attachedItem);
		ATTACHEDITEM_INFO* info = Allocate(currentLevel, sizeof(ATTACHEDITEM_INFO), 1);
		info->item_carrier = l->item_number;
		info->node = LM_RHAND;
		info->object = obj;
		attached->data = info;
		l->pose_count = attachedItem;
		AddActiveItem(attachedItem);
	}
}

void RemoveKeyPuzzleObject(ITEM_INFO* item) {
	if(item->object_number == LARA) {
		LARA_INFO* l = (LARA_INFO*)item->data;
		if(l->pose_count != NO_ITEM) {
			KillItem(l->pose_count);
		}
	}
}

void (*effect_routines[47])(ITEM_INFO* item) = {
	turn180_effect,
	floor_shake_effect,
	PoseidonSFX,
	LaraBubbles,
	finish_level_effect,
	ActivateCamera,
	ActivateKey,
	RubbleFX,
	SwapCrowbar,
	void_effect,
	SoundFlipEffect,
	ExplosionFX,
	lara_hands_free,
	void_effect,
	draw_right_gun,
	draw_left_gun,
	shoot_right_gun,
	shoot_left_gun,
	swap_meshes_with_meshswap1,
	swap_meshes_with_meshswap2,
	swap_meshes_with_meshswap3,
	invisibility_on,
	invisibility_off,
	void_effect,
	void_effect,
	void_effect,
	reset_hair,
	void_effect,
	SetFog,
	GhostTrap,
	LaraLocation,
	ClearScarabsPatch,
	AddFootPrint,
	AttachKeyPuzzleObject,
	RemoveKeyPuzzleObject,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	void_effect,
	MeshSwapToPour,
	MeshSwapFromPour,
	LaraLocationPad,
	KillActiveBaddies,
};