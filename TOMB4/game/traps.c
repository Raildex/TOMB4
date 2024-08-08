#include "game/traps.h"
#include "game/animstruct.h"
#include "game/camera.h"
#include "game/collide.h"
#include "game/collinfo.h"
#include "game/control.h"
#include "game/delstuff.h"
#include "game/deltapak.h"
#include "game/draw.h"
#include "game/dripstruct.h"
#include "game/effect2.h"
#include "game/effects.h"
#include "game/floorinfo.h"
#include "game/fxinfo.h"
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
#include "game/larawaterstatus.h"
#include "game/levelinfo.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/roomflags.h"
#include "game/roominfo.h"
#include "game/rope.h"
#include "game/smokesparks.h"
#include "game/sound.h"
#include "game/sparks.h"
#include "game/sphere.h"
#include "game/switch.h"
#include "game/tomb4fx.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/input.h"
#include "specific/output.h"
#include <stdlib.h>


short SPxzoffs[8] = { 0, 0, 0x200, 0, 0, 0, -0x200, 0 };
short SPyoffs[8] = { -0x400, 0, -0x200, 0, 0, 0, -0x200, 0 };
short SPDETyoffs[8] = { 0x400, 0x200, 0x200, 0x200, 0, 0x200, 0x200, 0x200 };

static unsigned char Flame3xzoffs[16][2]
	= { { 9, 9 }, { 24, 9 }, { 40, 9 }, { 55, 9 }, { 9, 24 }, { 24, 24 }, { 40, 24 }, { 55, 24 }, { 9, 40 }, { 24, 40 }, { 40, 40 }, { 55, 40 }, { 9, 55 }, { 24, 55 }, { 40, 55 }, { 55, 55 } };

short floor_fires[16 * 3] = // 16 points on the burning floor that spawn fires!
	{
		// xoff, zoff, size
		-96, 1216, 2, 560, 736, 2, -432, -976, 2, -64, -128, 2,
		824, 64, 2, 456, -352, 1, 392, 352, 1, 1096, 608, 1,
		-424, -416, 1, 520, 1152, 1, -248, 516, 1, -808, 80, 1,
		-1192, -384, 0, -904, -864, 0, -136, -912, 0, 184, 608, 0
	};

short deadly_floor_fires[4 * 2] = // 4 points on the burning floor that kill
								  // Lara if she is too close at explode time
	{
		// xoff, zoff
		-512, -512, 0, 0, 512, 512, 0, 768
	};

static PHD_VECTOR FloorTrapDoorPos = { 0, 0, -655 };
static PHD_VECTOR CeilingTrapDoorPos = { 0, 1056, -480 };
static short FloorTrapDoorBounds[12]
	= { -256, 256, 0, 0, -1024, -256, -1820, 1820, -5460, 5460, -1820, 1820 };
static short CeilingTrapDoorBounds[12]
	= { -256, 256, 0, 900, -768, -256, -1820, 1820, -5460, 5460, -1820, 1820 };

char LibraryTab[8];

void FlameEmitterControl(short item_number) {
	ITEM_INFO* item;
	unsigned long distance;
	long x, z;

	item = GetItem(currentLevel, item_number);

	if(!TriggerActive(item)) {
		if(item->trigger_flags >= 0) {
			LibraryTab[item->trigger_flags] = 0;
		}

		return;
	}

	if(item->trigger_flags < 0) {
		if((-item->trigger_flags & 7) == 2 || (-item->trigger_flags & 7) == 7) {
			SoundEffect(SFX_FLAME_EMITTER, (PHD_VECTOR*)&item->pos, item->room_number, 0);
			TriggerSuperJetFlame(
				item, -256 - (3072 * GlobalCounter & 0x1C00),
				GlobalCounter & 1);
			TriggerDynamic(
				item->pos.pos.x, item->pos.pos.y, item->pos.pos.z,
				(GetRandomControl() & 3) + 20,
				(GetRandomControl() & 0x3F) + 192,
				(GetRandomControl() & 0x1F) + 96, 0);
		} else {
			if(item->item_flags[0]) {
				if(item->item_flags[1]) {
					item->item_flags[1] -= item->item_flags[1] >> 2;
				}

				if(item->item_flags[2]) {
					item->item_flags[2] += 8;
				}

				item->item_flags[0]--;

				if(!item->item_flags[0]) {
					item->item_flags[3] = (GetRandomControl() & 0x3F) + 150;
				}
			} else {
				item->item_flags[3]--;

				if(!item->item_flags[3]) {
					if(-item->trigger_flags >> 3) {
						item->item_flags[0] = (GetRandomControl() & 0x1F)
							+ 30 * (-item->trigger_flags >> 3);
					} else {
						item->item_flags[0] = (GetRandomControl() & 0x3F) + 60;
					}
				}

				if(item->item_flags[2]) {
					item->item_flags[2] -= 8;
				}

				if(item->item_flags[1] > -8192) {
					item->item_flags[1] -= 512;
				}
			}

			if(item->item_flags[2]) {
				AddFire(
					item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, 0,
					item->room_number, item->item_flags[2] & 0xFF);
			}

			if(item->item_flags[1]) {
				SoundEffect(SFX_FLAME_EMITTER, (PHD_VECTOR*)&item->pos, item->room_number, 0);

				if(item->item_flags[1] > -8192) {
					TriggerSuperJetFlame(
						item, item->item_flags[1], GlobalCounter & 1);
				} else {
					TriggerSuperJetFlame(
						item, -256 - (3072 * GlobalCounter & 0x1C00),
						GlobalCounter & 1);
				}

				TriggerDynamic(
					item->pos.pos.x, item->pos.pos.y, item->pos.pos.z,
					(-item->item_flags[1] >> 10) - (GetRandomControl() & 1)
						+ 16,
					(GetRandomControl() & 0x3F) + 192,
					(GetRandomControl() & 0x1F) + 96, 0);
			} else {
				TriggerDynamic(
					item->pos.pos.x, item->pos.pos.y, item->pos.pos.z,
					10 - (GetRandomControl() & 1),
					(GetRandomControl() & 0x3F) + 192,
					(GetRandomControl() & 0x1F) + 96, 0);
			}
		}

		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, (PHD_VECTOR*)&item->pos, item->room_number, 0);
	} else {
		LibraryTab[item->trigger_flags] = 1;
		AddFire(
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, 2,
			item->room_number, 0);
		TriggerDynamic(
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z,
			16 - (GetRandomControl() & 1), (GetRandomControl() & 0x3F) + 192,
			(GetRandomControl() & 0x1F) + 96, 0);
		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);

		if(!lara.burn && ItemNearLara(&item->pos, 600)) {
			x = lara_item->pos.pos.x - item->pos.pos.x;
			z = lara_item->pos.pos.z - item->pos.pos.z;
			distance = SQUARE(x) + SQUARE(z);

			if(distance < 0x40000) {
				LaraBurn();
			}
		}
	}
}

static long OnTwoBlockPlatform(ITEM_INFO* item, long x, long z) {
	long tx, tz;

	if(!item->mesh_bits) {
		return 0;
	}

	x >>= 10;
	z >>= 10;
	tx = item->pos.pos.x >> 10;
	tz = item->pos.pos.z >> 10;

	if(!item->pos.y_rot && (x == tx || x == tx - 1) && (z == tz || z == tz + 1)) {
		return 1;
	}

	if(item->pos.y_rot == 0x8000 && (x == tx || x == tx + 1)
	   && (z == tz || z == tz - 1)) {
		return 1;
	}

	if(item->pos.y_rot == 0x4000 && (z == tz || z == tz - 1)
	   && (x == tx || x == tx + 1)) {
		return 1;
	}

	if(item->pos.y_rot == -0x4000 && (z == tz || z == tz - 1)
	   && (x == tx || x == tx - 1)) {
		return 1;
	}

	return 0;
}

void TwoBlockPlatformFloor(
	ITEM_INFO* item, long x, long y, long z, long* height, height_types* height_type, long* tiltxoff, long* tiltzoff, long* OnObject) {
	if(OnTwoBlockPlatform(item, x, z)) {
		if(y <= item->pos.pos.y + 32 && item->pos.pos.y < *height) {
			*height = item->pos.pos.y;
			*OnObject = 1;
			*height_type = WALL;
		}
	}
}

void TwoBlockPlatformCeiling(
	ITEM_INFO* item, long x, long y, long z, long* height) {
	if(OnTwoBlockPlatform(item, x, z)) {
		if(y > item->pos.pos.y + 32 && item->pos.pos.y > *height) {
			*height = item->pos.pos.y + 256;
		}
	}
}

void ControlTwoBlockPlatform(short item_number) {
	ITEM_INFO* item;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	long height;
	short room_number;

	item = GetItem(currentLevel, item_number);

	if(!TriggerActive(item)) {
		return;
	}

	if(item->trigger_flags) {
		if(item->pos.pos.y > item->item_flags[0]
			   - ((long)(item->trigger_flags & 0xFFFFFFF0) << 4)) {
			item->pos.pos.y -= item->trigger_flags & 0xF;
		}

		room_number = item->room_number;
		item->floor = GetHeight(
			GetFloor(
				item->pos.pos.x, item->pos.pos.y, item->pos.pos.z,
				&room_number),
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

		if(room_number != item->room_number) {
			ItemNewRoom(item_number, room_number);
		}
	} else {
		OnObject = 0;
		height = lara_item->pos.pos.y + 1;
		TwoBlockPlatformFloor(
			item, lara_item->pos.pos.x, lara_item->pos.pos.y,
			lara_item->pos.pos.z, &height, &height_type, &tiltxoff, &tiltzoff, &OnObject);

		if(!OnObject || lara_item->anim_number == 89) {
			item->item_flags[1] = -1;
		} else {
			item->item_flags[1] = 1;
		}

		if(item->item_flags[1] > 0) {
			if(item->pos.pos.y >= item->item_flags[0] + 128) {
				item->item_flags[1] = -1;
			} else {
				SoundEffect(SFX_RUMBLE_NEXTDOOR, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
				item->pos.pos.y += 4;
			}
		} else if(item->item_flags[1] < 0) {
			if(item->pos.pos.y <= item->item_flags[0]) {
				item->item_flags[1] = 1;
			} else {
				SoundEffect(SFX_RUMBLE_NEXTDOOR, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
				item->pos.pos.y -= 4;
			}
		}
	}
}

void ControlJobySpike(short item_number) {
	ITEM_INFO* item;
	PHD_VECTOR dir;
	short* frm[2];
	long rate, y, h;

	item = GetItem(currentLevel, item_number);

	if(TriggerActive(item)) {
		SoundEffect(SFX_METAL_SCRAPE_LOOP, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
		GetFrames(lara_item, frm, &rate);
		y = lara_item->pos.pos.y + frm[0][2];
		h = item->pos.pos.y + (3328 * item->item_flags[1] >> 12);

		if(lara_item->hit_points > 0 && h > y
			&& abs(item->pos.pos.x - lara_item->pos.pos.x) < 512
			&& abs(item->pos.pos.z - lara_item->pos.pos.z) < 512) {
			dir.x = GetRandomControl() & 0x3F;
			dir.y = -256;
			dir.z = GetRandomControl() & 0x3F;
			Normalise(&dir);
			DoBloodSplat(
				lara_item->pos.pos.x + (GetRandomControl() & 0x7F) - 64,
				GetRandomControl() % (h - y) + y,
				lara_item->pos.pos.z + (GetRandomControl() & 0x7F) - 64,
				(GetRandomControl() & 3) + 2, (short)(2 * GetRandomControl()), dir,
				item->room_number);
			lara_item->hit_points -= 8;
		}

		if(!item->item_flags[2]) {
			if(item->item_flags[0] < 4096) {
				item->item_flags[0] += (item->item_flags[0] >> 6) + 2;
			}
		} else if(item->item_flags[0] > -4096) {
			item->item_flags[0] += (item->item_flags[0] >> 6) - 2;
		}

		if(item->item_flags[1] < item->item_flags[3]) {
			item->item_flags[1] += 3;
		}

		item->pos.y_rot += item->item_flags[0];
	}
}

void DrawScaledSpike(ITEM_INFO* item) {
	PHD_VECTOR scale;
	ROOM_INFO* r;
	short** meshpp;
	short* frm[2];
	long rate, clip, lp;

	if(item->object_number != TEETH_SPIKES || item->item_flags[1]) {
		if((item->object_number == RAISING_BLOCK1
			|| item->object_number == RAISING_BLOCK2)
		   && item->trigger_flags && !item->item_flags[0]) {
			for(lp = 1; lp < 8; lp++) {
				if(!LibraryTab[lp]) {
					break;
				}
			}

			if(lp == 8) {
				item->item_flags[0] = 1;
				item->touch_bits = 0;
				AddActiveItem(GetItemNum(currentLevel, item));
				item->flags |= IFL_CODEBITS;
				item->status = ITEM_ACTIVE;
			}
		}

		r = GetRoom(currentLevel, item->room_number);
		phd_left = r->left;
		phd_right = r->right;
		phd_top = r->top;
		phd_bottom = r->bottom;
		GetFrames(item, frm, &rate);
		phd_PushMatrix();
		phd_TranslateAbs(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z);
		phd_RotX(item->pos.x_rot);
		phd_RotZ(item->pos.z_rot);
		phd_RotY(item->pos.y_rot);
		clip = S_GetObjectInfoBounds(frm[0]);

		if(clip) {
			meshpp = GetMeshPointer(
				currentLevel,
				GetObjectInfo(currentLevel, item->object_number)->mesh_index);

			if(item->object_number == EXPANDING_PLATFORM) {
				scale.x = 16384;
				scale.y = 16384;
				scale.z = item->item_flags[1] << 2;
			} else {
				scale.y = item->item_flags[1] << 2;

				if(item->object_number != JOBY_SPIKES) {
					scale.x = 16384;
					scale.z = 16384;
				} else {
					scale.x = 12288;
					scale.z = 12288;
				}
			}

			ScaleCurrentMatrix(&scale);
			CalculateObjectLighting(item, frm[0]);
			phd_PutPolygons(*meshpp, clip);
		}

		phd_left = 0;
		phd_right = phd_winwidth;
		phd_top = 0;
		phd_bottom = phd_winheight;
		phd_PopMatrix();
	}
}

void ControlSlicerDicer(short item_number) {
	ITEM_INFO* item;
	long distance;
	short room_number;

	item = GetItem(currentLevel, item_number);
	SoundEffect(SFX_METAL_SCRAPE_LOOP, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
	SoundEffect(SFX_METAL_SCRAPE_LOOP1, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
	distance = 4608 * phd_cos(item->trigger_flags) >> W2V_SHIFT;
	item->pos.pos.x = 256 * item->item_flags[0]
		+ (phd_sin(item->pos.y_rot) * distance >> W2V_SHIFT);
	item->pos.pos.y = 256 * item->item_flags[1]
		- (4608 * phd_sin(item->trigger_flags) >> W2V_SHIFT);
	item->pos.pos.z = 256 * item->item_flags[2]
		+ (phd_cos(item->pos.y_rot) * distance >> W2V_SHIFT);
	item->trigger_flags += 170;
	room_number = item->room_number;
	GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);

	if(item->room_number != room_number) {
		ItemNewRoom(item_number, room_number);
	}

	AnimateItem(item);
}

void ControlSprinkler(short item_number) {
	ITEM_INFO* item;
	DRIP_STRUCT* drip;
	SPARKS* sptr;
	SMOKE_SPARKS* smokeptr;
	long vel, size;

	item = GetItem(currentLevel, item_number);

	if(item->item_flags[0] < 1200) {
		item->item_flags[0]++;

		if(!(wibble & 0xF)
		   && (item->item_flags[0] <= 600
			   || GetRandomControl() % (item->item_flags[0] - 600) < 100)) {
			drip = GetFreeDrip();
			drip->pos.x = (GetRandomControl() & 0x1F) + item->pos.pos.x - 16;
			drip->pos.y = (GetRandomControl() & 0x1F) + item->pos.pos.y - 944;
			drip->pos.z = (GetRandomControl() & 0x1F) + item->pos.pos.z - 16;
			drip->On = 1;
			drip->R = 112;
			drip->G = (GetRandomControl() & 0x1F) + 128;
			drip->B = (GetRandomControl() & 0x1F) + 128;
			drip->Yvel = (GetRandomControl() & 0xF) + 16;
			drip->Gravity = (GetRandomControl() & 0x1F) + 32;
			drip->Life = (GetRandomControl() & 0x1F) + 16;
			drip->RoomNumber = item->room_number;
		}
	}

	if(item->item_flags[0] <= 600) {
		SoundEffect(SFX_SANDHAM_IN_THE_HOUSE, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);

		for(int i = 0; i < 3; i++) {
			sptr = GetFreeSpark();
			sptr->On = 1;
			sptr->sR = 112;
			sptr->sG = (GetRandomControl() & 0x1F) + 128;
			sptr->sB = (GetRandomControl() & 0x1F) + 128;
			sptr->dR = sptr->sR >> 1;
			sptr->dG = sptr->sG >> 1;
			sptr->dB = sptr->sB >> 1;
			sptr->ColFadeSpeed = 4;
			sptr->FadeToBlack = 8;
			sptr->Life = 20;
			sptr->sLife = 20;
			sptr->TransType = 2;
			vel = ((GlobalCounter & 3) << 10) + (GetRandomControl() & 0x3FF);
			sptr->Xvel = -rcossin_tbl[vel << 1] >> 2;
			sptr->Xvel = (sptr->Xvel * (GetRandomControl() & 0xFF)) >> 8;
			sptr->Yvel = -32 - (GetRandomControl() & 0x1F);
			sptr->Zvel = rcossin_tbl[(vel << 1) + 1] >> 2;
			sptr->Zvel = (sptr->Zvel * (GetRandomControl() & 0xFF)) >> 8;
			sptr->x = item->pos.pos.x + (sptr->Xvel >> 3);
			sptr->y = (sptr->Yvel >> 5) + item->pos.pos.y - 928;
			sptr->z = item->pos.pos.z + (sptr->Zvel >> 3);
			sptr->Friction = 4;
			sptr->Flags = GetRandomControl() & 0x20;
			sptr->Gravity = (GetRandomControl() & 0x3F) + 64;
			sptr->MaxYvel = 0;
		}

		for(int i = 0; i < 1; i++) {
			smokeptr = GetFreeSmokeSpark();
			smokeptr->On = 1;
			smokeptr->sShade = 0;
			smokeptr->dShade = (GetRandomControl() & 0x1F) + 32;
			smokeptr->ColFadeSpeed = 4;
			smokeptr->FadeToBlack = 8 - (GetRandomControl() & 3);
			smokeptr->TransType = 2;
			smokeptr->Life = (GetRandomControl() & 3) + 24;
			smokeptr->sLife = smokeptr->Life;
			smokeptr->pos.x = (GetRandomControl() & 0x1F) + item->pos.pos.x - 16;
			smokeptr->pos.y = (GetRandomControl() & 0x1F) + item->pos.pos.y - 944;
			smokeptr->pos.z = (GetRandomControl() & 0x1F) + item->pos.pos.z - 16;
			smokeptr->Xvel = 2 * (GetRandomControl() & 0x1FF) - 512;

			if(i) {
				smokeptr->Yvel = (GetRandomControl() & 0x1F) - 16;
			} else {
				smokeptr->Yvel = 2 * (GetRandomControl() & 0x1FF) + 512;
			}

			smokeptr->Zvel = 2 * (GetRandomControl() & 0x1FF) - 512;
			smokeptr->Friction = 3;
			smokeptr->Flags = 16;
			smokeptr->RotAng = GetRandomControl() & 0xFFF;

			if(GetRandomControl() & 1) {
				smokeptr->RotAdd = -64 - (GetRandomControl() & 0x3F);
			} else {
				smokeptr->RotAdd = (GetRandomControl() & 0x3F) + 64;
			}

			smokeptr->MaxYvel = 0;
			smokeptr->Gravity = -4 - (GetRandomControl() & 3);
			size = (GetRandomControl() & 0xF) + 16;

			if(!i) {
				size <<= 2;
			}

			smokeptr->dSize = (unsigned char)size;
			smokeptr->sSize = smokeptr->dSize >> 3;
			smokeptr->Size = smokeptr->dSize >> 3;
		}
	}
}

void ControlMineHelicopter(short item_number) {
	ITEM_INFO* item;
	ITEM_INFO* sentry;
	SPHERE* sphere;
	long nSpheres;
	short sentries, fade;

	item = GetItem(currentLevel, item_number);
	nSpheres = GetSpheres(item, Slist, 1);

	if(item->item_flags[0] < 150) {
		item->item_flags[0]++;
		fade = item->item_flags[0] * 4;

		if(fade > 255) {
			fade = 0;
		}

		for(int i = 0; i < nSpheres; i++) {
			sphere = &Slist[i];

			if(!i || i > 5) {
				AddFire(
					sphere->x, sphere->y, sphere->z, 2, item->room_number,
					fade);
			}
		}

		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
	} else {
		SoundEffect(SFX_EXPLOSION1, (PHD_VECTOR*)&item->pos,item->room_number, SFX_DEFAULT);
		SoundEffect(SFX_EXPLOSION2, (PHD_VECTOR*)&item->pos,item->room_number, SFX_DEFAULT);
		SoundEffect(SFX_EXPLOSION1, (PHD_VECTOR*)&item->pos,item->room_number, 0x1800000 | SFX_SETPITCH);

		for(int i = 0; i < nSpheres; i++) {
			sphere = &Slist[i];

			if(i >= 7 && i != 9) {
				TriggerExplosionSparks(
					sphere->x, sphere->y, sphere->y, 3, -2, 0,
					-item->room_number);
				TriggerExplosionSparks(
					sphere->x, sphere->y, sphere->y, 3, -1, 0,
					-item->room_number);
				TriggerShockwave(
					(PHD_VECTOR*)&sphere->x, 0x1300030,
					(GetRandomControl() & 0x1F) + 112, 0x20806000, 0x800);
			}
		}

		for(int i = 0; i < nSpheres; i++) {
			ExplodeItemNode(item, i, 0, -128);
		}

		FlashFadeR = 255;
		FlashFadeG = 192;
		FlashFadeB = 64;
		FlashFader = 32;

		for(sentries = GetRoom(currentLevel, item->room_number)->item_number;
			sentries != NO_ITEM; sentries = sentry->next_item) {
			sentry = GetItem(currentLevel, sentries);

			if(sentry->object_number == SENTRY_GUN) {
				sentry->mesh_bits &= ~0x40;
			}
		}

		KillItem(item_number);
	}
}

void MineCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll) {
	ITEM_INFO* item;
	ITEM_INFO* mines;

	item = GetItem(currentLevel, item_number);

	if(item->trigger_flags || item->item_flags[3]) {
		return;
	}

	if(l->anim_number == ANIM_MINEDETECT
	   && l->frame_number
		   >= GetAnim(currentLevel, ANIM_MINEDETECT)->frame_base + 57) {
		for(int i = 0; i < GetNumLevelItems(currentLevel); i++) {
			mines = GetItem(currentLevel, i);

			if(mines->object_number != MINE || mines->status == ITEM_INVISIBLE
			   || mines->trigger_flags) {
				continue;
			}

			TriggerExplosionSparks(
				mines->pos.pos.x, mines->pos.pos.y, mines->pos.pos.z, 3, -2, 0,
				mines->room_number);

			for(int j = 0; j < 2; j++) {
				TriggerExplosionSparks(
					mines->pos.pos.x, mines->pos.pos.y, mines->pos.pos.z, 3, -1,
					0, mines->room_number);
			}

			mines->mesh_bits = 1;
			ExplodeItemNode(mines, 0, 0, -32);
			KillItem(i);

			if(!(GetRandomControl() & 3)) {
				SoundEffect(SFX_MINE_EXP_OVERLAY, (PHD_VECTOR*)&mines->pos, mines->room_number, SFX_DEFAULT);
			}

			mines->status = ITEM_INVISIBLE;
		}
	} else if(TestBoundsCollide(item, l, 512)) {
		TriggerExplosionSparks(
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, 3, -2, 0,
			item->room_number);

		for(int i = 0; i < 2; i++) {
			TriggerExplosionSparks(
				item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, 3, -1, 0,
				item->room_number);
		}

		item->mesh_bits = 1;
		ExplodeItemNode(item, 0, 0, 128);
		KillItem(item_number);
		l->anim_number = ANIM_MINEDEATH;
		l->frame_number = GetAnim(currentLevel, ANIM_MINEDEATH)->frame_base;
		l->current_anim_state = AS_DEATH;
		l->speed = 0;
		SoundEffect(SFX_MINE_EXP_OVERLAY, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
	}
}

void FallingSquishyBlockCollision(
	short item_number, ITEM_INFO* l, COLL_INFO* coll) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l)) {
		if(item->frame_number
			   - GetAnim(currentLevel, item->anim_number)->frame_base
		   <= 8) {
			item->frame_number += 2;
			l->hit_points = 0;
			l->current_anim_state = AS_DEATH;
			l->goal_anim_state = AS_DEATH;
			l->anim_number = ANIM_FBLOCK_DEATH;
			l->frame_number
				= GetAnim(currentLevel, ANIM_FBLOCK_DEATH)->frame_base + 50;
			l->fallspeed = 0;
			l->speed = 0;

			for(int i = 0; i < 12; i++) {
				TriggerBlood(
					l->pos.pos.x, l->pos.pos.y - 128, l->pos.pos.z,
					GetRandomControl() << 1, 3);
			}
		} else if(l->hit_points > 0) {
			ItemPushLara(item, l, coll, 0, 1);
		}
	}
}

void ControlFallingSquishyBlock(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(TriggerActive(item)) {
		if(item->item_flags[0] < 60) {
			SoundEffect(SFX_EARTHQUAKE_LOOP, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
			camera.bounce = (item->item_flags[0] - 92) >> 1;
			item->item_flags[0]++;
		} else {
			if(item->frame_number
				   - GetAnim(currentLevel, item->anim_number)->frame_base
			   == 8) {
				camera.bounce = -96;
			}

			AnimateItem(item);
		}
	}
}

void ControlLRSquishyBlock(short item_number) {
	ITEM_INFO* item;
	unsigned short ang;
	short frame;

	item = GetItem(currentLevel, item_number);

	if(!TriggerActive(item)) {
		return;
	}

	frame = item->frame_number
		- GetAnim(currentLevel, item->anim_number)->frame_base;

	if(item->touch_bits) {
		ang = (unsigned short)phd_atan(
				  item->pos.pos.z - lara_item->pos.pos.z,
				  item->pos.pos.x - lara_item->pos.pos.x)
			- item->pos.y_rot;

		if(!frame && ang > 0xA000 && ang < 0xE000) {
			item->item_flags[0] = 9;
			lara_item->hit_points = 0;
			lara_item->pos.y_rot = item->pos.y_rot - 0x4000;
		} else if(frame == 33 && ang > 0x2000 && ang < 0x6000) {
			item->item_flags[0] = 42;
			lara_item->hit_points = 0;
			lara_item->pos.y_rot = item->pos.y_rot + 0x4000;
		}
	}

	if(!item->item_flags[0] || frame != item->item_flags[0]) {
		AnimateItem(item);
	}
}

void ControlSethBlade(short item_number) {
	ITEM_INFO* item;
	short frame;

	item = GetItem(currentLevel, item_number);
	*(long*)&item->item_flags[0] = 0;

	if(!TriggerActive(item)) {
		return;
	}

	if(item->current_anim_state == 2) {
		if(item->item_flags[2] > 1) {
			item->item_flags[2]--;
		} else if(item->item_flags[2] == 1) {
			item->goal_anim_state = 1;
			item->item_flags[2] = 0;
		} else if(!item->item_flags[2] && item->trigger_flags > 0) {
			item->item_flags[2] = item->trigger_flags;
		}
	} else {
		frame = item->frame_number
			- GetAnim(currentLevel, item->anim_number)->frame_base;

		if(frame && frame <= 6) {
			*(long*)&item->item_flags[0] = -1;
		} else if(frame >= 7 && frame <= 15) {
			*(long*)&item->item_flags[0] = 448;
		} else {
			*(long*)&item->item_flags[0] = 0;
		}

		item->item_flags[3] = 1000;
	}

	AnimateItem(item);
}

void ControlPlinthBlade(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(TriggerActive(item)) {
		if(item->frame_number
		   == GetAnim(currentLevel, item->anim_number)->frame_end) {
			item->item_flags[3] = 0;
		} else {
			item->item_flags[3] = 200;
		}

		AnimateItem(item);
	} else {
		item->frame_number
			= GetAnim(currentLevel, item->anim_number)->frame_base;
	}
}

void ControlMovingBlade(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(TriggerActive(item)) {
		item->item_flags[3] = 50;
		AnimateItem(item);
	} else {
		item->frame_number
			= GetAnim(currentLevel, item->anim_number)->frame_base;
	}
}

void ControlCatwalkBlade(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(TriggerActive(item)) {
		if(item->frame_number
			   == GetAnim(currentLevel, item->anim_number)->frame_end
		   || item->frame_number
				   - GetAnim(currentLevel, item->anim_number)->frame_base
			   < 38) {
			item->item_flags[3] = 0;
		} else {
			item->item_flags[3] = 100;
		}

		AnimateItem(item);
	} else {
		item->frame_number
			= GetAnim(currentLevel, item->anim_number)->frame_base;
	}
}

void ControlBirdBlade(short item_number) {
	ITEM_INFO* item;
	short frame;

	item = GetItem(currentLevel, item_number);
	item->item_flags[3] = 100;

	if(TriggerActive(item)) {
		frame = item->frame_number
			- GetAnim(currentLevel, item->anim_number)->frame_base;

		if(frame <= 14 || frame >= 31) {
			*(long*)&item->item_flags[0] = 0;
		} else {
			*(long*)&item->item_flags[0] = 6;
		}

		AnimateItem(item);
	} else {
		item->frame_number
			= GetAnim(currentLevel, item->anim_number)->frame_base;
		*(long*)&item->item_flags[0] = 0;
	}
}

void Control4xFloorRoofBlade(short item_number) {
	ITEM_INFO* item;
	short frame;

	item = GetItem(currentLevel, item_number);

	if(TriggerActive(item)) {
		frame = item->frame_number
			- GetAnim(currentLevel, item->anim_number)->frame_base;

		if(frame <= 5 || frame >= 58 || frame >= 8 && frame <= 54) {
			*(long*)&item->item_flags[0] = 0;
		} else {
			if(frame > 7) {
				item->item_flags[3] = 200;
			} else {
				item->item_flags[3] = 20;
			}

			*(long*)&item->item_flags[0] = 30;
		}

		AnimateItem(item);
	} else {
		item->frame_number
			= GetAnim(currentLevel, item->anim_number)->frame_base;
		*(long*)&item->item_flags[0] = 0;
	}
}

void ControlSpikeball(short item_number) {
	ITEM_INFO* item;
	short frame;

	item = GetItem(currentLevel, item_number);
	frame = item->frame_number
		- GetAnim(currentLevel, item->anim_number)->frame_base;

	if(TriggerActive(item)) {
		if((frame <= 14 || frame >= 24) && (frame < 138 || frame > 140)) {
			if(frame < 141) {
				*(long*)&item->item_flags[0] = 0;
			} else {
				item->item_flags[3] = 50;
				*(long*)&item->item_flags[0] = 0x7FF800;
			}
		} else {
			item->item_flags[3] = 150;
			*(long*)&item->item_flags[0] = 0x7FF800;
		}

		AnimateItem(item);
	} else {
		item->frame_number
			= GetAnim(currentLevel, item->anim_number)->frame_base;
		*(long*)&item->item_flags[0] = 0;
	}
}

void ControlHammer(short item_number) {
	ITEM_INFO* item;
	ITEM_INFO* item2;
	long hammered; // hammer touched a pushable (Senet lose path)
	short frame, target_item;

	item = GetItem(currentLevel, item_number);
	frame = item->frame_number
		- GetAnim(currentLevel, item->anim_number)->frame_base;
	item->item_flags[3] = 150;

	if(!TriggerActive(item)) {
		*(long*)&item->item_flags[0] = 0;
		return;
	}

	hammered = 0;

	if(!item->trigger_flags) {
		if(frame < 52) {
			*(long*)&item->item_flags[0] = 0xE0;
		} else {
			*(long*)&item->item_flags[0] = 0;
		}
	} else if(item->current_anim_state == 1 && item->goal_anim_state == 1) {
		if(item->item_flags[2]) {
			if(item->trigger_flags == 3) {
				item->flags &= ~IFL_CODEBITS;
				item->item_flags[2] = 0;
			} else if(item->trigger_flags == 4) {
				item->item_flags[2]--;
			} else {
				item->item_flags[2] = 0;
			}
		} else {
			item->anim_number
				= GetObjectInfo(currentLevel, HAMMER)->anim_index + 1;
			item->frame_number
				= GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 2;
			item->goal_anim_state = 2;
			item->item_flags[2] = 60;
		}
	} else {
		item->goal_anim_state = 1;

		if(frame < 52) {
			*(long*)&item->item_flags[0] = 0x7E0;
		} else {
			*(long*)&item->item_flags[0] = 0;
		}

		if(frame == 8) {
			if(item->trigger_flags == 2) {
				for(target_item
					= GetRoom(currentLevel, item->room_number)->item_number;
					target_item != NO_ITEM; target_item = item2->next_item) {
					item2 = GetItem(currentLevel, target_item);

					if(item2->object_number == OBELISK
					   && item2->pos.y_rot == -0x4000
					   && GetItem(currentLevel, item2->item_flags[0])->pos.y_rot == 0x4000
					   && !GetItem(currentLevel, item2->item_flags[1])->pos.y_rot) {
						item2->flags |= IFL_CODEBITS;
						GetItem(currentLevel, item2->item_flags[0])->flags
							|= IFL_CODEBITS;
						GetItem(currentLevel, item2->item_flags[1])->flags
							|= IFL_CODEBITS;
						break;
					}
				}

				SoundEffect(SFX_DOOR_GEN_THUD, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
				SoundEffect(SFX_EXPLOSION2, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
			} else {
				for(target_item
					= GetRoom(currentLevel, item->room_number)->item_number;
					target_item != NO_ITEM; target_item = item2->next_item) {
					item2 = GetItem(currentLevel, target_item);

					if(item2->object_number >= PUSHABLE_OBJECT1
					   && item2->object_number <= PUSHABLE_OBJECT4
					   && item2->pos.pos.x == item->pos.pos.x
					   && item2->pos.pos.z == item->pos.pos.z) {
						ExplodeItemNode(item2, 0, 0, 128);
						KillItem(target_item);
						hammered = 1;
					}
				}

				if(hammered) {
					for(target_item
						= GetRoom(currentLevel, item->room_number)->item_number;
						target_item != NO_ITEM;
						target_item = item2->next_item) {
						item2 = GetItem(currentLevel, target_item);

						if(item2->object_number == PUZZLE_ITEM4_COMBO1
						   || item2->object_number == PUZZLE_ITEM4_COMBO2
						   || item2->object_number == PUZZLE_ITEM5) {
							if(item2->pos.pos.x == item->pos.pos.x
							   && item2->pos.pos.z == item->pos.pos.z) {
								item2->status = ITEM_INACTIVE;
							}
						}
					}
				}
			}
		} else if(frame > 52 && item->trigger_flags == 2) {
			item->flags &= ~IFL_CODEBITS;
		}
	}

	AnimateItem(item);
}

void ControlStargate(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);
	item->item_flags[3] = 50;

	if(TriggerActive(item)) {
		SoundEffect(SFX_STARGATE_SWIRL, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
		*(long*)&item->item_flags[0] = 0x36DB600;
		AnimateItem(item);
	} else {
		*(long*)&item->item_flags[0] = 0;
	}
}

void ControlPlough(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);
	item->item_flags[3] = 50;

	if(TriggerActive(item)) {
		*(long*)&item->item_flags[0] = 0x3F000;
		AnimateItem(item);
	} else {
		*(long*)&item->item_flags[0] = 0;
	}
}

void ControlChain(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(item->trigger_flags) {
		item->item_flags[2] = 1;
		item->item_flags[3] = 75;

		if(TriggerActive(item)) {
			*(long*)&item->item_flags[0] = 0x3F000;
			AnimateItem(item);
			return;
		}
	} else {
		item->item_flags[3] = 25;

		if(TriggerActive(item)) {
			*(long*)&item->item_flags[0] = 0x780;
			AnimateItem(item);
			return;
		}
	}

	*(long*)&item->item_flags[0] = 0;
}

void ControlBurningFloor(short item_number) {
	ITEM_INFO* item;
	ITEM_INFO* torch;
	SPHERE* sphere;
	long nSpheres, dx, dy, dz;
	short torch_num, xoff, zoff, size;

	item = GetItem(currentLevel, item_number);

	if(!item->item_flags[3]) {
		nSpheres = 0;
		torch_num = GetRoom(currentLevel, item->room_number)->item_number;

		while(1) {
			torch = GetItem(currentLevel, torch_num);

			if(torch->object_number == BURNING_TORCH_ITEM && !torch->speed
			   && !torch->fallspeed && torch->item_flags[3]) {
				if(!nSpheres) {
					nSpheres = GetSpheres(item, Slist, 1);

					for(int i = 0; i < nSpheres; i++) {
						sphere = &Slist[i];
						dx = sphere->x - torch->pos.pos.x;
						dy = sphere->y - torch->pos.pos.y;
						dz = sphere->z - torch->pos.pos.z;

						if(SQUARE(dx) + SQUARE(dy) + SQUARE(dz)
						   > SQUARE(sphere->r + 32)) {
							item->item_flags[3] = 1;
							KillItem(torch_num);
							return;
						}
					}
				}
			}

			torch_num = torch->next_item;

			if(torch_num == NO_ITEM) {
				return;
			}
		}
	}

	for(int i = 0; i < 15; i++) {
		xoff = floor_fires[(i * 3) + 0];
		zoff = floor_fires[(i * 3) + 1];
		size = floor_fires[(i * 3) + 2];

		if(item->item_flags[size]) {
			AddFire(
				item->pos.pos.x + xoff, item->pos.pos.y - (size << 6) - 64,
				item->pos.pos.z + zoff, size, item->room_number,
				item->item_flags[size]);
		}
	}

	if(!lara.burn) {
		for(int i = 0; i < 3; i++) {
			xoff = deadly_floor_fires[(i * 2) + 0];
			zoff = deadly_floor_fires[(i * 2) + 1];
			dx = abs(item->pos.pos.x + xoff - lara_item->pos.pos.x);
			dy = abs(item->pos.pos.y - lara_item->pos.pos.y);
			dz = abs(
				item->pos.pos.z + zoff
				- lara_item->pos.pos.z); // ORIGINAL BUG uses xoff instead of
										 // zoff, only affects last test

			if(dx < 200 && dy < 200 && dz < 200) {
				LaraBurn();
				lara_item->hit_points = 100;
				item->item_flags[3] = 450;
				item->item_flags[0] = 2;
				item->item_flags[1] = 2;
				item->item_flags[2] = 2;
				FlashFadeR = 255;
				FlashFadeG = 64;
				FlashFadeB = 0;
				FlashFader = 32;
			}
		}
	}

	if(item->item_flags[3] < 450) {
		item->item_flags[0] += 4;

		if(item->item_flags[3] > 30) {
			item->item_flags[1] += 4;
		}

		if(item->item_flags[3] > 60) {
			item->item_flags[2] += 8;
		}

		if(item->item_flags[0] > 255) {
			item->item_flags[0] = 255;
		}

		if(item->item_flags[1] > 255) {
			item->item_flags[1] = 255;
		}

		if(item->item_flags[2] > 255) {
			item->item_flags[2] = 255;
		}

		item->item_flags[3]++;
		item->required_anim_state = 127 - item->item_flags[3] / 6;
	} else {
		item->item_flags[0] -= 4;
		item->item_flags[1] -= 3;
		item->item_flags[2] -= 2;

		if(item->item_flags[0] < 2) {
			item->item_flags[0] = 2;
		}

		if(item->item_flags[1] < 2) {
			item->item_flags[1] = 2;
		}

		if(item->item_flags[2] < 2) {
			item->item_flags[2] = 2;
		}

		if(item->item_flags[0] == 2 && item->item_flags[1] == 2
		   && item->item_flags[2] == 2) {
			FlipMap(0);
			ExplodeItemNode(item, 0, 1, -24);
			ExplodeItemNode(item, 1, 1, -24);
			ExplodeItemNode(item, 2, 1, -24);
			ExplodeItemNode(item, 3, 1, -24);
			ExplodeItemNode(item, 4, 1, -32);
			KillItem(item_number);
		}
	}
}

void ControlRaisingBlock(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(TriggerActive(item)) {
		if(!item->item_flags[2]) {
			if(item->object_number == RAISING_BLOCK2) {
				AlterFloorHeight(item, -2048);
			} else {
				AlterFloorHeight(item, -1024);
			}

			item->item_flags[2] = 1;
		}

		if(item->item_flags[1] < 4096) {
			SoundEffect(SFX_RUMBLE_NEXTDOOR, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
			item->item_flags[1] += 64;

			if(item->trigger_flags
			   && abs(item->pos.pos.x - lara_item->pos.pos.x) < 10240
			   && abs(item->pos.pos.y - lara_item->pos.pos.y) < 10240
			   && abs(item->pos.pos.z - lara_item->pos.pos.z) < 10240) {
				if(item->item_flags[1] == 64 || item->item_flags[1] == 4096) {
					camera.bounce = -32;
				} else {
					camera.bounce = -16;
				}
			}
		}
	} else if(item->item_flags[1] > 0) {
		SoundEffect(SFX_RUMBLE_NEXTDOOR, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);

		if(item->trigger_flags
		   && abs(item->pos.pos.x - lara_item->pos.pos.x) < 10240
		   && abs(item->pos.pos.y - lara_item->pos.pos.y) < 10240
		   && abs(item->pos.pos.z - lara_item->pos.pos.z) < 10240) {
			if(item->item_flags[1] == 64 || item->item_flags[1] == 4096) {
				camera.bounce = -32;
			} else {
				camera.bounce = -16;
			}
		}

		item->item_flags[1] -= 64;
	} else if(item->item_flags[2]) {
		if(item->object_number == RAISING_BLOCK2) {
			AlterFloorHeight(item, 2048);
		} else {
			AlterFloorHeight(item, 1024);
		}

		item->item_flags[2] = 0;
	}
}

void ControlScaledSpike(short item_number) {
	ITEM_INFO* item;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	short* bounds;
	short* larabounds;
	long dx, dy, dz, num;
	short room_number, yt, yb, iyb1, iyb2, hit;

	item = GetItem(currentLevel, item_number);

	if(!TriggerActive(item) || item->item_flags[2]) {
		if(TriggerActive(item)) {
			item->item_flags[0] += (item->item_flags[0] >> 3) + 32;
			item->item_flags[1] -= item->item_flags[0];

			if(item->item_flags[1] < 0) {
				item->item_flags[0] = 1024;
				item->item_flags[1] = 0;
				item->status = ITEM_INVISIBLE;
			}

			if(item->trigger_flags & 32) {
				item->item_flags[2] = 1;
			} else if(item->item_flags[2]) {
				item->item_flags[2]--;
			}
		} else if(!item->timer) {
			item->item_flags[0] += (item->item_flags[0] >> 3) + 32;

			if(item->item_flags[1] > 0) {
				item->item_flags[1] -= item->item_flags[0];

				if(item->item_flags[1] < 0) {
					item->item_flags[1] = 0;
				}
			}
		}
	} else {
		if(item->item_flags[0] == 1024) {
			SoundEffect(SFX_TEETH_SPIKES, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
		}

		item->status = ITEM_ACTIVE;
		hit = (short)TestBoundsCollideTeethSpikes(item);

		if(lara_item->hit_points > 0 && hit) {
			bounds = GetBestFrame(item);
			larabounds = GetBestFrame(lara_item);
			num = 0;

			if((item->item_flags[0] > 1024 || lara_item->gravity_status)
			   && (item->trigger_flags & 7) > 2
			   && (item->trigger_flags & 7) < 6) {
				if(lara_item->fallspeed > 6 || item->item_flags[0] > 1024) {
					lara_item->hit_points = -1;
					num = 20;
				}
			} else {
				lara_item->hit_points -= 8;
				num = (GetRandomControl() & 3) + 2;
			}

			yt = (short)(item->pos.pos.y + larabounds[2]);
			yb = (short)(item->pos.pos.y + larabounds[3]);

			if((item->trigger_flags & 0xF) == 8
			   || !(item->trigger_flags & 0xF)) {
				iyb1 = -bounds[3];
				iyb2 = -bounds[2];
			} else {
				iyb1 = bounds[2];
				iyb2 = bounds[3];
			}

			if(yt < item->pos.pos.y + iyb1) {
				yt = (short)(iyb1 + item->pos.pos.y);
			}

			if(yb > item->pos.pos.y + iyb2) {
				yb = (short)(iyb2 + item->pos.pos.y);
			}

			dy = abs(yt - yb) + 1;

			if((item->trigger_flags & 7) == 2 || (item->trigger_flags & 7) == 6) {
				num >>= 1;
			}

			while(num--) {
				dx = (GetRandomControl() & 0x7F) + lara_item->pos.pos.x - 64;
				dz = (GetRandomControl() & 0x7F) + lara_item->pos.pos.z - 64;
				TriggerBlood(
					dx, yb - GetRandomControl() % dy, dz,
					GetRandomControl() << 1, 1);
			}

			if(lara_item->hit_points <= 0) {
				room_number = lara_item->room_number;
				dy = GetHeight(
					GetFloor(
						lara_item->pos.pos.x, lara_item->pos.pos.y,
						lara_item->pos.pos.z, &room_number),
					lara_item->pos.pos.x, lara_item->pos.pos.y,
					lara_item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

				if(item->pos.pos.y >= lara_item->pos.pos.y
				   && dy - lara_item->pos.pos.y < 50) {
					lara_item->anim_number = ANIM_SPIKED;
					lara_item->frame_number
						= GetAnim(currentLevel, ANIM_SPIKED)->frame_base;
					lara_item->current_anim_state = AS_DEATH;
					lara_item->goal_anim_state = AS_DEATH;
					lara_item->gravity_status = 0;
				}
			}
		}

		item->item_flags[0] += 128;
		item->item_flags[1] += item->item_flags[0];

		if(item->item_flags[1] >= 5120) {
			item->item_flags[1] = 5120;

			if(item->item_flags[0] <= 1024) {
				item->item_flags[0] = 0;

				if(!(item->trigger_flags & 16) && lara_item->hit_points > 0) {
					item->item_flags[2] = 64;
				}
			} else {
				item->item_flags[0] = -item->item_flags[0] >> 1;
			}
		}
	}
}

void FlameEmitter3Control(short item_number) {
	ITEM_INFO *item, *item2;
	PHD_3DPOS pos;
	PHD_VECTOR s, d;
	long x, z, distance, r, g, b;

	item = GetItem(currentLevel, item_number);

	if(!TriggerActive(item)) {
		return;
	}

	if(item->trigger_flags) {
		SoundEffect(SFX_ELEC_ARCING_LOOP, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
		g = (GetRandomControl() & 0x3F) + 192;
		b = (GetRandomControl() & 0x3F) + 192;
		s.x = item->pos.pos.x;
		s.y = item->pos.pos.y;
		s.z = item->pos.pos.z;

		if(!(GlobalCounter & 3)
		   && (item->trigger_flags == 2 || item->trigger_flags == 4)) {
			d.x = item->pos.pos.x
				+ (2048 * phd_sin(item->pos.y_rot - 0x8000) >> W2V_SHIFT);
			d.y = item->pos.pos.y;
			d.z = item->pos.pos.z
				+ (2048 * phd_cos(item->pos.y_rot - 0x8000) >> W2V_SHIFT);

			if(GetRandomControl() & 3) {
				TriggerLightning(
					&s, &d, (GetRandomControl() & 0x1F) + 64, RGBA(0, g, b, 24),
					0, 32, 3);
			} else {
				TriggerLightning(
					&s, &d, (GetRandomControl() & 0x1F) + 96, RGBA(0, g, b, 32),
					1, 32, 3);
			}
		}

		if(item->trigger_flags >= 3 && !(GlobalCounter & 1)) {
			d.x = 0;
			d.y = -64;
			d.z = 20;
			item2 = GetItem(
				currentLevel, item->item_flags[2 + (GlobalCounter >> 2 & 1)]);
			GetJointAbsPosition(item2, &d, 0);

			if(!(GlobalCounter & 3)) {
				if(GetRandomControl() & 3) {
					TriggerLightning(
						&s, &d, (GetRandomControl() & 0x1F) + 64,
						RGBA(0, g, b, 24), 0, 32, 5);
				} else {
					TriggerLightning(
						&s, &d, (GetRandomControl() & 0x1F) + 96,
						RGBA(0, g, b, 32), 1, 32, 5);
				}
			}

			if(item->trigger_flags != 3 || item2->trigger_flags) {
				TriggerLightningGlow(d.x, d.y, d.z, RGBA(0, g, b, 64));
			}
		}

		if((GlobalCounter & 3) == 2) {
			s.x = item->pos.pos.x;
			s.y = item->pos.pos.y;
			s.z = item->pos.pos.z;
			d.x = s.x + (GetRandomControl() & 0x1FF) - 256;
			d.y = s.y + (GetRandomControl() & 0x1FF) - 256;
			d.z = s.z + (GetRandomControl() & 0x1FF) - 256;
			TriggerLightning(
				&s, &d, (GetRandomControl() & 0xF) + 16, RGBA(0, g, b, 24), 3,
				32, 3);
			TriggerLightningGlow(s.x, s.y, s.z, RGBA(0, g, b, 32));
		}
	} else {
		if(!item->item_flags[0]) {
			item->item_flags[0] = (GetRandomControl() & 3) + 8;
			distance = GetRandomControl() & 0x3F;
			item->item_flags[1] = (short)(distance == item->item_flags[1] ? (distance + 13) & 0x3F
																		  : distance);
		} else {
			item->item_flags[0]--;
		}

		if(!(wibble & 4)) {
			x = 16 * (Flame3xzoffs[item->item_flags[1] & 7][0] - 32);
			z = 16 * (Flame3xzoffs[item->item_flags[1] & 7][1] - 32);
			TriggerFireFlame(
				item->pos.pos.x + x, item->pos.pos.y, item->pos.pos.z + z, -1,
				2);
		}

		if(wibble & 4) {
			x = 16 * (Flame3xzoffs[(item->item_flags[1] >> 3) + 8][0] - 32);
			z = 16 * (Flame3xzoffs[(item->item_flags[1] >> 3) + 8][1] - 32);
			TriggerFireFlame(
				item->pos.pos.x + x, item->pos.pos.y, item->pos.pos.z + z, -1,
				2);
		}

		SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
		distance = GetRandomControl();
		r = (distance & 0x3F) + 192;
		g = (distance >> 4 & 0x1F) + 96;
		TriggerDynamic(x, item->pos.pos.y, z, 12, r, g, 0);
		pos.pos.x = item->pos.pos.x;
		pos.pos.y = item->pos.pos.y;
		pos.pos.z = item->pos.pos.z;

		if(ItemNearLara(&pos, 600) && !lara.burn) {
			lara_item->hit_points -= 5;
			lara_item->hit_status = 1;
			x = lara_item->pos.pos.x - pos.pos.x;
			z = lara_item->pos.pos.z - pos.pos.z;
			distance = SQUARE(x) + SQUARE(z);

			if(distance < 202500) {
				LaraBurn();
			}
		}
	}
}

void FlameControl(short fx_number) {
	FX_INFO* fx;
	long r, g, b, wh;

	if(lara.water_status == LW_FLYCHEAT) {
		KillEffect(fx_number);
		lara.burn = 0;
		return;
	}

	fx = GetEffect(currentLevel, fx_number);

	for(int i = 14; i > 0; i--) {
		if(!(wibble & 0xC)) {
			fx->pos.pos.x = 0;
			fx->pos.pos.y = 0;
			fx->pos.pos.z = 0;
			GetLaraJointPos((PHD_VECTOR*)&fx->pos, i);
			TriggerFireFlame(
				fx->pos.pos.x, fx->pos.pos.y, fx->pos.pos.z, -1,
				255 - lara.BurnGreen);
		}
	}

	if(lara.BurnGreen) {
		r = GetRandomControl() & 0x3F;
		g = (GetRandomControl() & 0x3F) + 192;
		b = (GetRandomControl() & 0x1F) + 96;
		TriggerDynamic(
			lara_item->pos.pos.x, lara_item->pos.pos.y, lara_item->pos.pos.z,
			13, r, g, b);
	} else {
		r = (GetRandomControl() & 0x3F) + 192;
		g = (GetRandomControl() & 0x1F) + 96;
		TriggerDynamic(
			lara_item->pos.pos.x, lara_item->pos.pos.y, lara_item->pos.pos.z,
			13, r, g, 0);
	}

	if(lara_item->room_number != fx->room_number) {
		EffectNewRoom(fx_number, lara_item->room_number);
	}

	wh = GetWaterHeight(
		fx->pos.pos.x, fx->pos.pos.y, fx->pos.pos.z, fx->room_number);

	if(wh != NO_HEIGHT && fx->pos.pos.y > wh) {
		KillEffect(fx_number);
		lara.burn = 0;
		return;
	}

	SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, (PHD_VECTOR*)&fx->pos, fx->room_number, SFX_DEFAULT);
	lara_item->hit_points -= 7;
	lara_item->hit_status = 1;
}

void FlameEmitter2Control(short item_number) {
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	long r, g;
	short room_number;

	item = GetItem(currentLevel, item_number);

	if(!TriggerActive(item)) {
		return;
	}

	if(item->trigger_flags < 0) {
		if(!item->item_flags[0]) {
			FlipMap(-item->trigger_flags);
			flipmap[-item->trigger_flags] ^= IFL_CODEBITS;
			item->item_flags[0] = 1;
		}

		return;
	}

	if(item->trigger_flags != 2) {
		if(item->trigger_flags == 123) {
			AddFire(
				item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, 1,
				item->room_number, item->item_flags[3]);
		} else {
			AddFire(
				item->pos.pos.x, item->pos.pos.y, item->pos.pos.z,
				1 - item->trigger_flags, item->room_number,
				item->item_flags[3]);
		}
	}

	if(!item->trigger_flags || item->trigger_flags == 2) {
		r = (GetRandomControl() & 0x3F) + 192;
		g = (GetRandomControl() & 0x1F) + 96;

		if(item->item_flags[3]) {
			r = (r * item->item_flags[3]) >> 8;
			g = (g * item->item_flags[3]) >> 8;
		}

		TriggerDynamic(
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, 10, r, g, 0);
	}

	if(item->trigger_flags == 2) {
		item->pos.pos.x += phd_sin(item->pos.y_rot + 0x8000) >> 11;
		item->pos.pos.z += phd_cos(item->pos.y_rot + 0x8000) >> 11;
		room_number = item->room_number;
		floor = GetFloor(
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);

		if(GetRoom(currentLevel, room_number)->flags & ROOM_UNDERWATER) {
			FlashFadeR = 255;
			FlashFadeG = 128;
			FlashFadeB = 0;
			FlashFader = 32;
			KillItem(item_number);
			return;
		}

		if(item->room_number != room_number) {
			ItemNewRoom(item_number, room_number);
		}

		item->pos.pos.y = GetHeight(
			floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

		if(wibble & 7) {
			TriggerFireFlame(
				item->pos.pos.x, item->pos.pos.y - 32, item->pos.pos.z, -1, 1);
		}
	}

	SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
}

void LaraBurn() {
	short fx_number;

	if(!lara.burn && !lara.BurnGreen) {
		fx_number = CreateEffect(lara_item->room_number);

		if(fx_number != NO_ITEM) {
			GetEffect(currentLevel, fx_number)->object_number = FLAME;
			lara.burn = 1;
		}
	}
}

void LavaBurn(ITEM_INFO* item) {
	FLOOR_INFO* floor;
	short room_number;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;

	if(item->hit_points >= 0 && lara.water_status != LW_FLYCHEAT) {
		room_number = item->room_number;
		floor = GetFloor(item->pos.pos.x, 32000, item->pos.pos.z, &room_number);

		if(item->floor
		   == GetHeight(floor, item->pos.pos.x, 32000, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject)) {
			item->hit_status = 1;
			item->hit_points = -1;
			LaraBurn();
		}
	}
}

long TestBoundsCollideTeethSpikes(ITEM_INFO* item) {
	short* bounds;
	long x, y, z, rad, xMin, xMax, zMin, zMax;

	if(item->trigger_flags & 8) {
		x = item->pos.pos.x & ~0x3FF | 0x200;
		z = (item->pos.pos.z + SPxzoffs[item->trigger_flags & 7]) & ~0x3FF
			| 0x200;
	} else {
		x = (item->pos.pos.x - SPxzoffs[item->trigger_flags & 7]) & ~0x3FF
			| 0x200;
		z = item->pos.pos.z & ~0x3FF | 0x200;
	}

	if(item->trigger_flags & 1) {
		rad = 300;
	} else {
		rad = 480;
	}

	y = item->pos.pos.y + SPDETyoffs[item->trigger_flags & 7];
	bounds = GetBestFrame(lara_item);

	if(lara_item->pos.pos.y + bounds[2] > y
	   || lara_item->pos.pos.y + bounds[3] < y - 900) {
		return 0;
	}

	xMin = lara_item->pos.pos.x + bounds[0];
	xMax = lara_item->pos.pos.x + bounds[1];
	zMin = lara_item->pos.pos.z + bounds[4];
	zMax = lara_item->pos.pos.z + bounds[5];
	return xMin <= x + rad && xMax >= x - rad && zMin <= z + rad
		&& zMax >= z - rad;
}

void ControlRollingBall(short item_number) {
	ITEM_INFO* item;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	unsigned short tyrot, destyrot;
	short room_number, velnotadjusted;
	long h, fx, fz, fh, fhf, bz, bh, bhf, rx, rh, rhf, lx, lh, lhf;

	item = GetItem(currentLevel, item_number);

	if(!TriggerActive(item)) {
		return;
	}

	item->fallspeed += 6;
	item->pos.pos.x += item->item_flags[0] >> 5;
	item->pos.pos.y += item->fallspeed;
	item->pos.pos.z += item->item_flags[1] >> 5;
	room_number = item->room_number;
	h = GetHeight(
			GetFloor(
				item->pos.pos.x, item->pos.pos.y, item->pos.pos.z,
				&room_number),
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject)
		- 512;

	if(item->pos.pos.y > h) {
		if(abs(item->fallspeed) > 16) {
			fz = phd_sqrt(
				SQUARE(camera.pos.pos.x - item->pos.pos.x)
				+ SQUARE(camera.pos.pos.y - item->pos.pos.y)
				+ SQUARE(camera.pos.pos.z - item->pos.pos.z));

			if(fz < 0x4000) {
				camera.bounce
					= -(((0x4000 - fz) * abs(item->fallspeed)) >> W2V_SHIFT);
			}

			SoundEffect(SFX_BOULDER_FALL, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
		}

		if(item->pos.pos.y - h < 512) {
			item->pos.pos.y = h;
		}

		if(item->fallspeed > 64) {
			item->fallspeed = -(item->fallspeed >> 2);
		} else {
			if(abs(item->speed) <= 512 || GetRandomControl() & 0x1F) {
				item->fallspeed = 0;
			} else {
				item->fallspeed = -(GetRandomControl() % (item->speed >> 3));
			}
		}
	}

	fx = item->pos.pos.x;
	fz = item->pos.pos.z + 128;
	bz = item->pos.pos.z - 128;
	rx = item->pos.pos.x + 128;
	lx = item->pos.pos.x - 128;
	fh = GetHeight(
			 GetFloor(fx, item->pos.pos.y, fz, &room_number), fx,
			 item->pos.pos.y, fz, &height_type, &tiltxoff, &tiltzoff, &OnObject)
		- 512;
	bh = GetHeight(
			 GetFloor(fx, item->pos.pos.y, bz, &room_number), fx,
			 item->pos.pos.y, bz, &height_type, &tiltxoff, &tiltzoff, &OnObject)
		- 512;
	rh = GetHeight(
			 GetFloor(rx, item->pos.pos.y, bz + 128, &room_number), rx,
			 item->pos.pos.y, bz + 128, &height_type, &tiltxoff, &tiltzoff, &OnObject)
		- 512;
	lh = GetHeight(
			 GetFloor(lx, item->pos.pos.y, bz + 128, &room_number), lx,
			 item->pos.pos.y, bz + 128, &height_type, &tiltxoff, &tiltzoff, &OnObject)
		- 512;
	fx = item->pos.pos.x;
	fz = item->pos.pos.z + 512;
	bz = item->pos.pos.z - 512;
	rx = item->pos.pos.x + 512;
	lx = item->pos.pos.x - 512;
	fhf = GetHeight(
			  GetFloor(fx, item->pos.pos.y, fz, &room_number), fx,
			  item->pos.pos.y, fz, &height_type, &tiltxoff, &tiltzoff, &OnObject)
		- 512;
	bhf = GetHeight(
			  GetFloor(fx, item->pos.pos.y, bz, &room_number), fx,
			  item->pos.pos.y, bz, &height_type, &tiltxoff, &tiltzoff, &OnObject)
		- 512;
	rhf = GetHeight(
			  GetFloor(rx, item->pos.pos.y, bz + 512, &room_number), rx,
			  item->pos.pos.y, bz + 512, &height_type, &tiltxoff, &tiltzoff, &OnObject)
		- 512;
	lhf = GetHeight(
			  GetFloor(lx, item->pos.pos.y, bz + 512, &room_number), lx,
			  item->pos.pos.y, bz + 512, &height_type, &tiltxoff, &tiltzoff, &OnObject)
		- 512;

	if(item->pos.pos.y - h > -256 || item->pos.pos.y - fhf >= 512
	   || item->pos.pos.y - rhf >= 512 || item->pos.pos.y - bhf >= 512
	   || item->pos.pos.y - lhf >= 512) {
		velnotadjusted = 0;

		if(fh - h <= 256) {
			if(fhf - h < -1024 || fh - h < -256) {
				if(item->item_flags[1] <= 0) {
					if(!item->item_flags[1] && item->item_flags[0]) {
						item->pos.pos.z = (item->pos.pos.z & -512) | 512;
					}
				} else {
					item->item_flags[1] = -item->item_flags[1] >> 1;
					item->pos.pos.z = (item->pos.pos.z & -512) | 512;
				}
			} else if(fh == h) {
				velnotadjusted++;
			} else {
				item->item_flags[1] += (short)((fh - h) >> 1);
			}
		}

		if(bh - h > 256) {
			velnotadjusted++;
		} else if(bhf - h < -1024 || bh - h < -256) {
			if(item->item_flags[1] >= 0) {
				if(!item->item_flags[1] && item->item_flags[0]) {
					item->pos.pos.z = (item->pos.pos.z & -512) | 512;
				}
			} else {
				item->item_flags[1] = -item->item_flags[1] >> 1;
				item->pos.pos.z = (item->pos.pos.z & -512) | 512;
			}
		} else if(bh == h) {
			velnotadjusted++;
		} else {
			item->item_flags[1] -= (short)((bh - h) >> 1);
		}

		if(velnotadjusted == 2) {
			if(abs(item->item_flags[1]) <= 64) {
				item->item_flags[1] = 0;
			} else {
				item->item_flags[1] -= item->item_flags[1] >> 6;
			}
		}

		velnotadjusted = 0;

		if(lh - h <= 256) {
			if(lhf - h < -1024 || lh - h < -256) {
				if(item->item_flags[0] >= 0) {
					if(!item->item_flags[0] && item->item_flags[1]) {
						item->pos.pos.x = (item->pos.pos.x & -512) | 512;
					}
				} else {
					item->item_flags[0] = -item->item_flags[0] >> 1;
					item->pos.pos.x = (item->pos.pos.x & -512) | 512;
				}
			} else if(lh == h) {
				velnotadjusted++;
			} else {
				item->item_flags[0] -= (short)((lh - h) >> 1);
			}
		}

		if(rh - h <= 256) {
			if(rhf - h < -1024 || rh - h < -256) {
				if(item->item_flags[0] <= 0) {
					if(!item->item_flags[0] && item->item_flags[1]) {
						item->pos.pos.x = (item->pos.pos.x & -512) | 512;
					}
				} else {
					item->item_flags[0] = -item->item_flags[0] >> 1;
					item->pos.pos.x = (item->pos.pos.x & -512) | 512;
				}
			} else if(rh == h) {
				velnotadjusted++;
			} else {
				item->item_flags[0] += (short)((rh - h) >> 1);
			}
		}

		if(velnotadjusted == 2) {
			if(abs(item->item_flags[0]) <= 64) {
				item->item_flags[0] = 0;
			} else {
				item->item_flags[0] -= item->item_flags[0] >> 6;
			}
		}
	}

	GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);

	if(item->room_number != room_number) {
		ItemNewRoom(item_number, room_number);
	}

	if(item->item_flags[0] > 3072) {
		item->item_flags[0] = 3072;
	} else if(item->item_flags[0] < -3072) {
		item->item_flags[0] = -3072;
	}

	if(item->item_flags[1] > 3072) {
		item->item_flags[1] = 3072;
	} else if(item->item_flags[1] < -3072) {
		item->item_flags[1] = -3072;
	}

	tyrot = item->pos.y_rot;

	if(item->item_flags[1] || item->item_flags[0]) {
		destyrot = (unsigned short)(phd_atan(
			item->item_flags[0], item->item_flags[0]));
	} else {
		destyrot = item->pos.y_rot;
	}

	if(tyrot != destyrot) {
		if(((destyrot - tyrot) & 0x7FFF) >= 512) {
			if(destyrot <= tyrot || destyrot - tyrot >= 32768) {
				item->pos.y_rot = tyrot - 512;
			} else {
				item->pos.y_rot = tyrot + 512;
			}
		} else {
			item->pos.y_rot = destyrot;
		}
	}

	item->pos.x_rot
		-= (abs(item->item_flags[0]) + abs(item->item_flags[1])) >> 1;
	GetHeight(
		GetFloor(
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number),
		item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	TestTriggers(trigger_index, 1, 0);
}

void RollingBallCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(!TestBoundsCollide(item, l, coll->radius) || !TestCollision(item, l)) {
		return;
	}

	if(TriggerActive(item) && (item->item_flags[0] || item->fallspeed)) {
		lara_item->anim_number = ANIM_RBALL_DEATH;
		lara_item->frame_number
			= GetAnim(currentLevel, ANIM_RBALL_DEATH)->frame_base;
		lara_item->current_anim_state = AS_DEATH;
		lara_item->goal_anim_state = AS_DEATH;
		lara_item->gravity_status = 0;
	} else {
		ObjectCollision(item_number, l, coll);
	}
}

void DartsControl(short item_number) {
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	PHD_VECTOR dir;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	long x, z, speed;
	short room_num;

	item = GetItem(currentLevel, item_number);

	if(item->touch_bits) {
		lara_item->hit_points -= 25;
		lara_item->hit_status = 1;
		lara.poisoned += 160;
		dir.x = item->pos.x_rot;
		dir.y = item->pos.y_rot;
		dir.z = item->pos.z_rot;
		DoBloodSplat(
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z,
			(GetRandomControl() & 3) + 4, lara_item->pos.y_rot, dir,
			lara_item->room_number);
		KillItem(item_number);
	} else {
		x = item->pos.pos.x;
		z = item->pos.pos.z;
		speed = (item->speed * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
		item->pos.pos.x += (speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
		item->pos.pos.y
			-= (item->speed * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
		item->pos.pos.z += (speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
		room_num = item->room_number;
		floor = GetFloor(
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_num);

		if(item->room_number != room_num) {
			ItemNewRoom(item_number, room_num);
		}

		item->floor = GetHeight(
			floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

		if(item->pos.pos.y >= item->floor) {
			for(int i = 0; i < 4; i++) {
				TriggerDartSmoke(x, item->pos.pos.y, z, 0, 0, 1);
			}

			KillItem(item_number);
		}
	}
}

void DartEmitterControl(short item_number) {
	ITEM_INFO* item;
	ITEM_INFO* dart;
	long x, z, xLimit, zLimit, xv, zv, rand;
	short num;

	item = GetItem(currentLevel, item_number);

	if(item->active) {
		if(item->timer > 0) {
			item->timer--;
			return;
		}

		item->timer = 24;
	}

	num = CreateItem();

	if(num != NO_ITEM) {
		x = 0;
		z = 0;
		dart = GetItem(currentLevel, num);
		dart->object_number = DARTS;
		dart->room_number = item->room_number;

		if(!item->pos.y_rot) {
			z = 512;
		} else if(item->pos.y_rot == 0x4000) {
			x = 512;
		} else if(item->pos.y_rot == -0x4000) {
			x = -512;
		} else if(item->pos.y_rot == -0x8000) {
			z = -512;
		}

		dart->pos.pos.x = x + item->pos.pos.x;
		dart->pos.pos.y = item->pos.pos.y - 512;
		dart->pos.pos.z = z + item->pos.pos.z;
		InitialiseItem(num);
		dart->pos.x_rot = 0;
		dart->pos.y_rot = item->pos.y_rot + 0x8000;
		dart->speed = 256;
		xLimit = 0;
		zLimit = 0;

		if(x) {
			xLimit = abs(x << 1) - 1;
		} else {
			zLimit = abs(z << 1) - 1;
		}

		for(int i = 0; i < 5; i++) {
			rand = -GetRandomControl();

			if(z >= 0) {
				zv = zLimit & rand;
			} else {
				zv = -(zLimit & rand);
			}

			if(x >= 0) {
				xv = xLimit & rand;
			} else {
				xv = -(xLimit & rand);
			}

			TriggerDartSmoke(
				dart->pos.pos.x, dart->pos.pos.y, dart->pos.pos.z, xv, zv, 0);
		}

		AddActiveItem(num);
		dart->status = ITEM_ACTIVE;
		SoundEffect(SFX_DART_SPITT, (PHD_VECTOR*)&dart->pos, dart->room_number, SFX_DEFAULT);
	}
}

void FallingCeiling(short item_number) {
	ITEM_INFO* item;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	short room_number;

	item = GetItem(currentLevel, item_number);

	if(!item->current_anim_state) {
		item->gravity_status = 1;
		item->goal_anim_state = 1;
	} else if(item->current_anim_state == 1 && item->touch_bits) {
		lara_item->hit_points -= 300;
		lara_item->hit_status = 1;
	}

	AnimateItem(item);

	if(item->status == ITEM_DEACTIVATED) {
		RemoveActiveItem(item_number);
	} else {
		room_number = item->room_number;
		item->floor = GetHeight(
			GetFloor(
				item->pos.pos.x, item->pos.pos.y, item->pos.pos.z,
				&room_number),
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

		if(room_number != item->room_number) {
			ItemNewRoom(item_number, room_number);
		}

		if(item->current_anim_state == 1 && item->pos.pos.y >= item->floor) {
			item->pos.pos.y = item->floor;
			item->goal_anim_state = 2;
			item->gravity_status = 0;
			item->fallspeed = 0;
		}
	}
}

void ControlSmashableBikeWall(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(!TriggerActive(item) || lara.vehicle == NO_ITEM) {
		return;
	}

	if(TestBoundsCollide(item, GetItem(currentLevel, lara.vehicle), 1024)) {
		SoundEffect(SFX_BIKE_HIT_OBJECTS, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
		item->mesh_bits = -2;
		ExplodingDeath2(item_number, -1, 2305);
		item->mesh_bits = 0;
		TestTriggersAtXYZ(
			item->pos.pos.x, item->pos.pos.y, item->pos.pos.z,
			item->room_number, 1, 0);
		KillItem(item_number);
	}
}

void ControlFallingBlock2(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(!TriggerActive(item)) {
		return;
	}

	if(item->pos.pos.y == lara_item->pos.pos.y
	   && OnTwoBlockPlatform(item, lara_item->pos.pos.x, lara_item->pos.pos.z)
	   && lara.vehicle != NO_ITEM) {
		SoundEffect(SFX_BIKE_HIT_OBJECTS, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
		item->mesh_bits = -2;
		ExplodingDeath2(item_number, -1, 417);
		KillItem(item_number);
	}
}

void FallingBlockCeiling(
	ITEM_INFO* item, long x, long y, long z, long* height) {
	long tx, tz;

	tx = x ^ item->pos.pos.x;
	tz = z ^ item->pos.pos.z;

	if(tx & ~1023 || tz & ~1023) {
		return;
	}

	if(y > item->pos.pos.y) {
		*height = item->pos.pos.y + 256;
	}
}

void FallingBlockFloor(ITEM_INFO* item, long x, long y, long z, long* height, height_types* height_type, long* tiltxoff, long* tiltzoff, long* OnObject) {
	long tx, tz;

	tx = x ^ item->pos.pos.x;
	tz = z ^ item->pos.pos.z;

	if(tx & ~1023 || tz & ~1023) {
		return;
	}

	if(y <= item->pos.pos.y) {
		*height = item->pos.pos.y;
		*height_type = WALL;
		*OnObject = 1;
	}
}

void FallingBlock(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(item->item_flags[0] <= 1) {
		item->mesh_bits = -2;
		ExplodingDeath2(item_number, -1, 2465);
		KillItem(item_number);
	} else {
		item->pos.x_rot = (GetRandomControl() & 0x3FF) - 512;
		item->pos.z_rot = (GetRandomControl() & 0x3FF) - 512;
		item->item_flags[0]--;
	}
}

void FallingBlockCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll) {
	ITEM_INFO* item;
	long x, z, tx, tz;

	item = GetItem(currentLevel, item_number);
	x = l->pos.pos.x;
	z = l->pos.pos.z;
	tx = item->pos.pos.x;
	tz = item->pos.pos.z;

	if(!item->item_flags[0] && !item->trigger_flags
	   && item->pos.pos.y == l->pos.pos.y && !((tx ^ x) & ~1023)
	   && !((z ^ tz) & ~1023)) {
		SoundEffect(SFX_ROCK_FALL_CRUMBLE, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
		AddActiveItem(item_number);
		item->item_flags[0] = 60;
		item->status = ITEM_ACTIVE;
		item->flags |= IFL_CODEBITS;
	}
}

void CeilingTrapDoorCollision(
	short item_number, ITEM_INFO* l, COLL_INFO* coll) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(S_IsActionDown(inputImpl, IN_ACTION) && item->status != ITEM_ACTIVE
	   && l->current_anim_state == AS_UPJUMP && l->gravity_status
	   && lara.gun_status == LG_NO_ARMS
	   && TestLaraPosition(CeilingTrapDoorBounds, item, l)) {
		AlignLaraPosition(&CeilingTrapDoorPos, item, l);
		lara.head_x_rot = 0;
		lara.head_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.torso_y_rot = 0;
		lara.gun_status = LG_HANDS_BUSY;
		l->gravity_status = 0;
		l->fallspeed = 0;
		l->anim_number = ANIM_PULLTRAP;
		l->frame_number = GetAnim(currentLevel, ANIM_PULLTRAP)->frame_base;
		l->current_anim_state = AS_PULLTRAP;
		AddActiveItem(item_number);
		item->status = ITEM_ACTIVE;
		item->goal_anim_state = 1;
		UseForcedFixedCamera = 1;
		ForcedFixedCamera.pos.x = item->pos.pos.x
			- ((1024 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT);
		ForcedFixedCamera.pos.y = item->pos.pos.y + 1024;
		ForcedFixedCamera.pos.z = item->pos.pos.z
			- ((1024 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT);
		ForcedFixedCamera.room_number = item->room_number;
	} else if(item->current_anim_state == 1) {
		UseForcedFixedCamera = 0;
	}
}

void FloorTrapDoorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll) {
	ITEM_INFO* item;
	long y;

	item = GetItem(currentLevel, item_number);

	if((S_IsActionDown(inputImpl, IN_ACTION) && item->status != ITEM_ACTIVE
		&& l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH
		&& lara.gun_status == LG_NO_ARMS)
	   || lara.IsMoving && lara.GeneralPtr == item_number) {
		if(TestLaraPosition(FloorTrapDoorBounds, item, l)) {
			if(MoveLaraPosition(&FloorTrapDoorPos, item, l)) {
				l->anim_number = ANIM_LIFTTRAP;
				l->frame_number
					= GetAnim(currentLevel, ANIM_LIFTTRAP)->frame_base;
				l->current_anim_state = AS_LIFTTRAP;
				lara.IsMoving = 0;
				lara.head_x_rot = 0;
				lara.head_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.torso_y_rot = 0;
				lara.gun_status = LG_HANDS_BUSY;
				AddActiveItem(item_number);
				item->goal_anim_state = 1;
				item->status = ITEM_ACTIVE;
				UseForcedFixedCamera = 1;
				ForcedFixedCamera.pos.x = item->pos.pos.x
					- ((2048 * phd_sin(item->pos.y_rot) >> W2V_SHIFT));
				ForcedFixedCamera.pos.z = item->pos.pos.z
					- ((2048 * phd_cos(item->pos.y_rot) >> W2V_SHIFT));
				y = item->pos.pos.y - 2048;

				if(y < GetRoom(currentLevel, item->room_number)->maxceiling) {
					y = GetRoom(currentLevel, item->room_number)->maxceiling;
				}

				ForcedFixedCamera.pos.y = y;
				ForcedFixedCamera.room_number = item->room_number;
			} else {
				lara.GeneralPtr = item_number;
			}
		}
	} else if(item->current_anim_state == 1) {
		UseForcedFixedCamera = 0;
	}
}

void OpenTrapDoor(ITEM_INFO* item) {
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	unsigned short pitsky;

	pitsky = item->item_flags[3];
	r = GetRoom(currentLevel, item->room_number);
	floor = &r->floor
				 [((item->pos.pos.z - r->z) >> 10)
				  + r->x_size * ((item->pos.pos.x - r->x) >> 10)];

	if(item->pos.pos.y == r->minfloor) {
		floor->pit_room = pitsky & 0xFF;
		r = GetRoom(currentLevel, floor->pit_room);
		floor = &r->floor
					 [((item->pos.pos.z - r->z) >> 10)
					  + r->x_size * ((item->pos.pos.x - r->x) >> 10)];
		floor->sky_room = pitsky >> 8;
	} else {
		floor->sky_room = pitsky >> 8;
		r = GetRoom(currentLevel, floor->sky_room);
		floor = &r->floor
					 [((item->pos.pos.z - r->z) >> 10)
					  + r->x_size * ((item->pos.pos.x - r->x) >> 10)];
		floor->pit_room = pitsky & 0xFF;
	}

	item->item_flags[2] = 0;
}

void CloseTrapDoor(ITEM_INFO* item) {
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	unsigned short pitsky;

	r = GetRoom(currentLevel, item->room_number);
	floor = &r->floor
				 [((item->pos.pos.z - r->z) >> 10)
				  + r->x_size * ((item->pos.pos.x - r->x) >> 10)];

	if(item->pos.pos.y == r->minfloor) {
		pitsky = floor->pit_room;
		floor->pit_room = 255;
		r = GetRoom(currentLevel, pitsky);
		floor = &r->floor
					 [((item->pos.pos.z - r->z) >> 10)
					  + r->x_size * ((item->pos.pos.x - r->x) >> 10)];
		pitsky |= floor->sky_room << 8;
		floor->sky_room = 255;
		item->item_flags[2] = 1;
		item->item_flags[3] = pitsky;
	} else if(item->pos.pos.y == r->maxceiling) {
		pitsky = floor->sky_room;
		floor->sky_room = 255;
		r = GetRoom(currentLevel, pitsky);
		floor = &r->floor
					 [((item->pos.pos.z - r->z) >> 10)
					  + r->x_size * ((item->pos.pos.x - r->x) >> 10)];
		pitsky <<= 8;
		pitsky |= floor->pit_room;
		floor->pit_room = 255;
		item->item_flags[2] = 1;
		item->item_flags[3] = pitsky;
	} else {
		item->item_flags[2] = 1;
		item->item_flags[3] = 0;
	}
}

void TrapDoorControl(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);

	if(TriggerActive(item)) {
		if(!item->current_anim_state && item->trigger_flags >= 0) {
			item->goal_anim_state = 1;
		}
	} else if(item->current_anim_state == 1) {
		item->goal_anim_state = 0;
	}

	AnimateItem(item);

	if(item->current_anim_state == 1 && item->item_flags[2]) {
		OpenTrapDoor(item);
	} else if(!item->current_anim_state && !item->item_flags[2]) {
		CloseTrapDoor(item);
	}
}

void ControlObelisk(short item_number) {
	ITEM_INFO* item;
	ITEM_INFO* pulley;
	ITEM_INFO* pyramid;
	ITEM_INFO* disc;
	PHD_VECTOR s;
	PHD_VECTOR d;
	long stop, rad;
	short r, g, b;

	item = GetItem(currentLevel, item_number);

	if(TriggerActive(item)) {
		if(item->item_flags[3] > 346) {
			return;
		}

		item->item_flags[3]++;
		r = (GetRandomControl() & 0x1F) + 224;
		g = r - (GetRandomControl() & 0x1F) - 32;
		b = g - (GetRandomControl() & 0x1F) - 128;

		if(!(GlobalCounter & 1)) {
			rad = 0x2000;

			if(item->item_flags[3] >= 256 || GetRandomControl() & 1) {
				if(item->item_flags[3] < 256 && !(GlobalCounter & 3)) {
					SoundEffect(SFX_ELEC_ONE_SHOT, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);
					rad = (GetRandomControl() & 0xFFF) + 3456;
				}

				s.x = item->pos.pos.x
					+ ((3456 * phd_sin(item->pos.y_rot + 0x4000)) >> W2V_SHIFT);
				s.y = item->pos.pos.y - 256;
				s.z = item->pos.pos.z
					+ ((3456 * phd_cos(item->pos.y_rot + 0x4000)) >> W2V_SHIFT);

				d.x = item->pos.pos.x
					+ ((rad * phd_sin(item->pos.y_rot + 0x4000)) >> W2V_SHIFT);
				d.y = item->pos.pos.y;
				d.z = item->pos.pos.z
					+ ((rad * phd_cos(item->pos.y_rot + 0x4000)) >> W2V_SHIFT);

				if(abs(s.x - lara_item->pos.pos.x) < 20480
				   && abs(s.y - lara_item->pos.pos.y) < 20480
				   && abs(s.z - lara_item->pos.pos.z) < 20480
				   && abs(d.x - lara_item->pos.pos.x) < 20480
				   && abs(d.y - lara_item->pos.pos.y) < 20480
				   && abs(d.z - lara_item->pos.pos.z) < 20480) {
					if(!(GlobalCounter & 3)) {
						TriggerLightning(
							&s, &d, (GetRandomControl() & 0x1F) + 32,
							RGBA(r, g, b, 24), 1, 32, 5);
					}

					TriggerLightningGlow(s.x, s.y, s.z, RGBA(r, g, b, 48));
				}
			}
		}

		if(item->item_flags[3] >= 256 && item->trigger_flags == 2) {
			s.x = item->pos.pos.x
				+ ((0x2000 * phd_sin(item->pos.y_rot + 0x4000)) >> W2V_SHIFT);
			s.y = item->pos.pos.y;
			s.z = item->pos.pos.z
				+ ((0x2000 * phd_cos(item->pos.y_rot + 0x4000)) >> W2V_SHIFT);
			SoundEffect(SFX_ELEC_ARCING_LOOP, &s, item->room_number, SFX_DEFAULT);

			if(GlobalCounter & 1) {
				d.x = (GetRandomControl() & 0x3FF) + s.x - 512;
				d.y = (GetRandomControl() & 0x3FF) + s.y - 512;
				d.z = (GetRandomControl() & 0x3FF) + s.z - 512;

				if(abs(s.x - lara_item->pos.pos.x) < 20480
				   && abs(s.y - lara_item->pos.pos.y) < 20480
				   && abs(s.z - lara_item->pos.pos.z) < 20480
				   && abs(d.x - lara_item->pos.pos.x) < 20480
				   && abs(d.y - lara_item->pos.pos.y) < 20480
				   && abs(d.z - lara_item->pos.pos.z) < 20480) {
					if(item->item_flags[2] != NO_ITEM) {
						pyramid = GetItem(currentLevel, item->item_flags[2]);
						ExplodeItemNode(pyramid, 0, 0, 128);
						KillItem(item->item_flags[2]);
						TriggerExplosionSparks(
							s.x, s.y, s.z, 3, -2, 0, pyramid->room_number);
						TriggerExplosionSparks(
							s.x, s.y, s.z, 3, -1, 0, pyramid->room_number);
						item->item_flags[2] = NO_ITEM;

						disc = find_a_fucking_item(PUZZLE_ITEM1_COMBO1);
						disc->status = ITEM_INACTIVE;
						SoundEffect(SFX_EXPLOSION1, (PHD_VECTOR*)&disc->pos,disc->room_number, SFX_DEFAULT);
						SoundEffect(SFX_EXPLOSION2, (PHD_VECTOR*)&disc->pos,disc->room_number, SFX_DEFAULT);
					}

					TriggerLightning(
						&s, &d, (GetRandomControl() & 0xF) + 16,
						RGBA(r, g, b, 24), 3, 24, 3);
					TriggerLightningGlow(s.x, s.y, s.z, RGBA(r, g, b, 64));
				}
			}
		}
	} else {
		AnimateItem(item);
		stop = 0;

		if(item->anim_number
		   == GetObjectInfo(currentLevel, item->object_number)->anim_index
			   + 2) // done going counter-clockwise
		{
			item->pos.y_rot -= 0x4000;

			if(S_IsActionDown(inputImpl, IN_ACTION)) {
				item->anim_number
					= GetObjectInfo(currentLevel, item->object_number)
						  ->anim_index
					+ 1;
				item->frame_number
					= GetAnim(currentLevel, item->anim_number)->frame_base;
			} else {
				stop = 1;
			}
		}

		if(item->anim_number
		   == GetObjectInfo(currentLevel, item->object_number)->anim_index
			   + 6) // done going clockwise
		{
			item->pos.y_rot += 0x4000;

			if(S_IsActionDown(inputImpl, IN_ACTION)) {
				item->anim_number
					= GetObjectInfo(currentLevel, item->object_number)
						  ->anim_index
					+ 5;
				item->frame_number
					= GetAnim(currentLevel, item->anim_number)->frame_base;
			} else {
				stop = 1;
			}
		}

		if(stop) {
			item->anim_number
				= GetObjectInfo(currentLevel, item->object_number)->anim_index
				+ 3;
			item->frame_number
				= GetAnim(currentLevel, item->anim_number)->frame_base;
		}

		if(item->trigger_flags == 2) {
			for(int i = 0; i < GetNumLevelItems(currentLevel); i++) {
				pulley = GetItem(currentLevel, i);

				if(pulley->object_number == PULLEY) {
					if(item->pos.y_rot == -0x4000
					   && GetItem(currentLevel, item->item_flags[0])->pos.y_rot
						   == 0x4000
					   && !GetItem(currentLevel, item->item_flags[1])
							   ->pos.y_rot) {
						pulley->item_flags[1] = 0;
					} else {
						pulley->item_flags[1] = 1;
					}

					break;
				}
			}
		}
	}
}
