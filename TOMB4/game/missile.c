
#include "game/missile.h"
#include "game/camera.h"
#include "game/control.h"
#include "game/debris.h"
#include "game/effects.h"
#include "game/fxinfo.h"
#include "game/heighttypes.h"
#include "game/items.h"
#include "game/levelinfo.h"
#include "game/shatteritem.h"
#include "game/sound.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include <stdlib.h>

long ExplodeFX(FX_INFO* fx, long NoXZVel, short Num) {
	short** meshpp;

	meshpp = GetMeshPointer(currentLevel, fx->frame_number);
	ShatterItem.YRot = fx->pos.y_rot;
	ShatterItem.meshp = *meshpp;
	ShatterItem.Sphere.x = fx->pos.pos.x;
	ShatterItem.Sphere.y = fx->pos.pos.y;
	ShatterItem.Sphere.z = fx->pos.pos.z;
	ShatterItem.Bit = 0;
	ShatterItem.Flags = fx->flag2 & 0x400;
	ShatterObject(&ShatterItem, 0, Num, fx->room_number, NoXZVel);
	return 1;
}

void ControlBodyPart(short fx_number) {
	FX_INFO* fx;
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	long height, ceiling, ox, oy, oz;
	short room_number;

	fx = GetEffect(currentLevel, fx_number);
	ox = fx->pos.pos.x;
	oz = fx->pos.pos.z;
	oy = fx->pos.pos.y;

	if(fx->speed) {
		fx->pos.x_rot += fx->fallspeed << 2;
	}

	fx->fallspeed += 6;

	fx->pos.pos.x += fx->speed * phd_sin(fx->pos.y_rot) >> W2V_SHIFT;
	fx->pos.pos.y += fx->fallspeed;
	fx->pos.pos.z += fx->speed * phd_cos(fx->pos.y_rot) >> W2V_SHIFT;
	room_number = fx->room_number;
	floor = GetFloor(fx->pos.pos.x, fx->pos.pos.y, fx->pos.pos.z, &room_number);
	ceiling = GetCeiling(floor, fx->pos.pos.x, fx->pos.pos.y, fx->pos.pos.z);

	if(fx->pos.pos.y < ceiling) {
		fx->pos.pos.y = ceiling;
		fx->fallspeed = -fx->fallspeed;
		fx->speed -= fx->speed >> 3;
	}

	height = GetHeight(floor, fx->pos.pos.x, fx->pos.pos.y, fx->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

	if(fx->pos.pos.y >= height) {
		if(fx->flag2 & 1) {
			fx->pos.pos.x = ox;
			fx->pos.pos.y = oy;
			fx->pos.pos.z = oz;

			if(fx->flag2 & 0x200) {
				ExplodeFX(fx, -2, 32);
			} else {
				ExplodeFX(fx, -1, 32);
			}

			KillEffect(fx_number);

			if(fx->flag2 & 0x800) {
				SoundEffect(SFX_ROCK_FALL_LAND, &fx->pos, SFX_DEFAULT);
			}

			return;
		}

		if(oy <= height) {
			if(fx->fallspeed <= 32) {
				fx->fallspeed = 0;
			} else {
				fx->fallspeed = -fx->fallspeed >> 2;
			}
		} else {
			fx->pos.y_rot += 32768;
			fx->pos.pos.x = ox;
			fx->pos.pos.z = oz;
		}

		fx->speed -= fx->speed >> 2;

		if(abs(fx->speed) < 4) {
			fx->speed = 0;
		}

		fx->pos.pos.y = oy;
	}

	if(!fx->speed) {
		fx->flag1++;

		if(fx->flag1 > 32) {
			KillEffect(fx_number);
			return;
		}
	}

	if(fx->flag2 & 2 && GetRandomControl() & 1) {
		DoBloodSplat((GetRandomControl() & 0x3F) + fx->pos.pos.x - 32, (GetRandomControl() & 0x1F) + fx->pos.pos.y - 16, (GetRandomControl() & 0x3F) + fx->pos.pos.z - 32, 1, (short)(GetRandomControl() << 1), fx->room_number);
	}

	if(room_number != fx->room_number) {
		EffectNewRoom(fx_number, room_number);
	}
}
