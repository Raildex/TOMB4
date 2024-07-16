
#include "game/effect2.h"
#include "game/control.h"
#include "game/croc.h"
#include "game/demigod.h"
#include "game/draw.h"
#include "game/dynamic.h"
#include "game/effects.h"
#include "game/fxinfo.h"
#include "game/gameflow.h"
#include "game/gamevector.h"
#include "game/gfleveloptions.h"
#include "game/harpy.h"
#include "game/heighttypes.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/lara.h"
#include "game/larainfo.h"
#include "game/levelinfo.h"
#include "game/missile.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/phd3dpos.h"
#include "game/ripplestruct.h"
#include "game/roomflags.h"
#include "game/roominfo.h"
#include "game/seth.h"
#include "game/sound.h"
#include "game/sparks.h"
#include "game/spdynamic.h"
#include "game/splashsetup.h"
#include "game/splashstruct.h"
#include "game/tomb4fx.h"
#include "game/traps.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "tomb4fx.h"
#include <stdlib.h>
#include <string.h>


DYNAMIC dynamics[MAX_DYNAMICS * 2];
long nSplashes;
SPLASH_STRUCT* splashes;
long nRipples;
RIPPLE_STRUCT* ripples;
SPLASH_SETUP splash_setup;
long nSpark;
SPARKS* spark;
long wibble = 0;
long SplashCount = 0;
long KillEverythingFlag = 0;
long SmokeCountL;
long SmokeCountR;
long SmokeWeapon;
long SmokeWindX;
long SmokeWindZ;

static SP_DYNAMIC spark_dynamics[8];
static long DeadlyBounds[6];
static long number_dynamics;

void ControlSmokeEmitter(short item_number) {
	ITEM_INFO* item;
	SPARKS* sptr;
	PHD_3DPOS pos;
	long size, dx, dz, normal;

	item = GetItem(currentLevel, item_number);
	normal = 0;

	if(!TriggerActive(item)) {
		return;
	}

	if(item->object_number == STEAM_EMITTER && GetRoom(currentLevel, item->room_number)->flags & ROOM_UNDERWATER) {
		if(item->item_flags[0] || !(GetRandomControl() & 0x1F) || item->trigger_flags == 1) {
			if(!(GetRandomControl() & 3) || item->item_flags[1]) {
				pos.pos.x = (GetRandomControl() & 0x3F) + item->pos.pos.x - 32;
				pos.pos.y = item->pos.pos.y - (GetRandomControl() & 0x1F) - 16;
				pos.pos.z = (GetRandomControl() & 0x3F) + item->pos.pos.z - 32;

				if(item->trigger_flags == 1) {
					CreateBubble(&pos, item->room_number, 15, 15);
				} else {
					CreateBubble(&pos, item->room_number, 8, 7);
				}

				if(item->item_flags[0]) {
					item->item_flags[0]--;

					if(!item->item_flags[0]) {
						item->item_flags[1] = 0;
					}
				}
			}
		} else if(!(GetRandomControl() & 0x1F)) {
			item->item_flags[0] = (GetRandomControl() & 3) + 4;
		}

		return;
	}

	if(item->object_number == STEAM_EMITTER && item->trigger_flags & 8) {
		if(item->item_flags[0]) {
			item->item_flags[0]--;

			if(!item->item_flags[0]) {
				item->item_flags[1] = (GetRandomControl() & 0x3F) + 30;
			}

			normal = 1;

			if(item->item_flags[2]) {
				item->item_flags[2] -= 256;
			}
		} else if(item->item_flags[2] < 4096) {
			item->item_flags[2] += 256;
		}

		if(item->item_flags[2]) {
			dx = lara_item->pos.pos.x - item->pos.pos.x;
			dz = lara_item->pos.pos.z - item->pos.pos.z;

			if(dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384) {
				return;
			}

			sptr = GetFreeSpark();
			sptr->On = 1;
			sptr->sR = 96;
			sptr->sG = 96;
			sptr->sB = 96;
			sptr->dR = 48;
			sptr->dG = 48;
			sptr->dB = 48;
			sptr->FadeToBlack = 6;
			sptr->ColFadeSpeed = (GetRandomControl() & 3) + 6;
			sptr->TransType = 2;
			sptr->Life = (GetRandomControl() & 7) + 16;
			sptr->sLife = sptr->Life;
			sptr->x = (GetRandomControl() & 0x3F) + item->pos.pos.x - 32;
			sptr->y = (GetRandomControl() & 0x3F) + item->pos.pos.y - 32;
			sptr->z = (GetRandomControl() & 0x3F) + item->pos.pos.z - 32;
			size = item->item_flags[2];

			if(item->item_flags[2] == 4096) {
				size = (GetRandomControl() & 0x7FF) + 2048;
			}

			sptr->Xvel = (short)((size * phd_sin(item->pos.y_rot - 32768)) >> W2V_SHIFT);
			sptr->Yvel = -16 - (GetRandomControl() & 0xF);
			sptr->Zvel = (short)((size * phd_cos(item->pos.y_rot - 32768)) >> W2V_SHIFT);
			sptr->Friction = 4;
			sptr->Flags = 538;

			if(!(GlobalCounter & 3)) {
				sptr->Flags = 1562;
			}

			sptr->RotAng = GetRandomControl() & 0xFFF;

			if(GetRandomControl() & 1) {
				sptr->RotAdd = -8 - (GetRandomControl() & 7);
			} else {
				sptr->RotAdd = (GetRandomControl() & 7) + 8;
			}

			sptr->Scalar = 2;
			sptr->Gravity = -8 - (GetRandomControl() & 0xF);
			sptr->MaxYvel = -8 - (GetRandomControl() & 7);
			size = (GetRandomControl() & 0x1F) + 128;
			sptr->dSize = (unsigned char)size;
			sptr->sSize = sptr->dSize >> 1;
			sptr->Size = sptr->sSize;

			if(item->item_flags[1]) {
				item->item_flags[1]--;
			} else {
				item->item_flags[0] = item->trigger_flags >> 4;
			}
		}

		if(!normal) {
			return;
		}
	}

	if(!(wibble & 0xF) && (item->object_number != STEAM_EMITTER || !(wibble & 0x1F))) {
		dx = lara_item->pos.pos.x - item->pos.pos.x;
		dz = lara_item->pos.pos.z - item->pos.pos.z;

		if(dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384) {
			return;
		}

		sptr = GetFreeSpark();
		sptr->On = 1;
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;

		if(item->object_number == SMOKE_EMITTER_BLACK) {
			sptr->dR = 96;
			sptr->dG = 96;
			sptr->dB = 96;
		} else {
			sptr->dR = 64;
			sptr->dG = 64;
			sptr->dB = 64;
		}

		sptr->FadeToBlack = 16;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
		sptr->Life = (GetRandomControl() & 7) + 28;
		sptr->sLife = sptr->Life;

		if(item->object_number == SMOKE_EMITTER_BLACK) {
			sptr->TransType = 3;
		} else {
			sptr->TransType = 2;
		}

		sptr->x = (GetRandomControl() & 0x3F) + item->pos.pos.x - 32;
		sptr->y = (GetRandomControl() & 0x3F) + item->pos.pos.y - 32;
		sptr->z = (GetRandomControl() & 0x3F) + item->pos.pos.z - 32;
		sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Yvel = -16 - (GetRandomControl() & 0xF);
		sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Friction = 3;
		sptr->Flags = 538;

		if(GetRoom(currentLevel, item->room_number)->flags & ROOM_OUTSIDE) {
			sptr->Flags = 794;
		}

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if(GetRandomControl() & 1) {
			sptr->RotAdd = -8 - (GetRandomControl() & 7);
		} else {
			sptr->RotAdd = (GetRandomControl() & 7) + 8;
		}

		sptr->Scalar = 2;
		sptr->Gravity = -8 - (GetRandomControl() & 0xF);
		sptr->MaxYvel = -8 - (GetRandomControl() & 7);
		size = (GetRandomControl() & 0x1F) + 128;
		sptr->dSize = (unsigned char)size;
		sptr->sSize = (unsigned char)(size >> 2);
		sptr->Size = (unsigned char)(size >> 2);

		if(item->object_number == STEAM_EMITTER) {
			sptr->Gravity >>= 1;
			sptr->Yvel >>= 1;
			sptr->MaxYvel >>= 1;
			sptr->Life += 16;
			sptr->sLife += 16;
			sptr->dR = 32;
			sptr->dG = 32;
			sptr->dB = 32;
		}
	}
}

void TriggerExplosionSmokeEnd(long x, long y, long z, long uw) {
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.pos.x - x;
	dz = lara_item->pos.pos.z - z;

	if(dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384) {
		return;
	}

	sptr = GetFreeSpark();
	sptr->On = 1;

	if(uw) {
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 192;
		sptr->dG = 192;
		sptr->dB = 208;
	} else {
		sptr->sR = 196;
		sptr->sG = 196;
		sptr->sB = 196;
		sptr->dR = 96;
		sptr->dG = 96;
		sptr->dB = 96;
	}

	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 64;
	sptr->Life = (GetRandomControl() & 0x1F) + 96;
	sptr->sLife = sptr->Life;

	if(uw) {
		sptr->TransType = 2;
	} else {
		sptr->TransType = 3;
	}

	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;

	if(uw) {
		sptr->Friction = 20;
		sptr->Yvel >>= 4;
		sptr->y += 32;
	} else {
		sptr->Friction = 6;
	}

	sptr->Flags = 538;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if(GetRandomControl() & 1) {
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	} else {
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}

	sptr->Scalar = 3;

	if(uw) {
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
	} else {
		sptr->Gravity = -3 - (GetRandomControl() & 3);
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	}

	sptr->dSize = (GetRandomControl() & 0x1F) + 128;
	sptr->sSize = sptr->dSize >> 2;
	sptr->Size = sptr->sSize;
}

void TriggerExplosionSmoke(long x, long y, long z, long uw) {
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.pos.x - x;
	dz = lara_item->pos.pos.z - z;

	if(dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384) {
		return;
	}

	sptr = GetFreeSpark();
	sptr->On = 1;

	if(!uw) {
		sptr->sR = 196;
		sptr->sG = 196;
		sptr->sB = 196;
		sptr->dR = 128;
		sptr->dG = 128;
		sptr->dB = 128;
	} else {
		sptr->sR = 144;
		sptr->sG = 144;
		sptr->sB = 144;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
	}

	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 8;
	sptr->TransType = 3;
	sptr->Life = (GetRandomControl() & 3) + 10;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1FF) + x - 256;
	sptr->y = (GetRandomControl() & 0x1FF) + y - 256;
	sptr->z = (GetRandomControl() & 0x1FF) + z - 256;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;

	if(uw) {
		sptr->Friction = 2;
	} else {
		sptr->Friction = 6;
	}

	sptr->Flags = 538;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->Scalar = 1;
	sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	sptr->Gravity = -3 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	sptr->dSize = (GetRandomControl() & 0x1F) + 128;
	sptr->sSize = sptr->dSize >> 2;
	sptr->Size = sptr->sSize >> 2;
	GetRandomControl(); // cool
}

void TriggerFlareSparks(long x, long y, long z, long xvel, long yvel, long zvel, long smoke) {
	SPARKS* sptr;
	SPARKS* smokeSpark;
	long dx, dz, rnd;

	dx = lara_item->pos.pos.x - x;
	dz = lara_item->pos.pos.z - z;

	if(dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000) {
		return;
	}

	rnd = GetRandomDraw();
	sptr = GetFreeSpark();
	sptr->On = 1;
	sptr->dR = -1;
	sptr->dG = (rnd & 0x7F) + 64;
	sptr->dB = -64 - sptr->dG;
	sptr->sR = -1;
	sptr->sG = -1;
	sptr->sB = -1;
	sptr->Life = 10;
	sptr->sLife = 10;
	sptr->ColFadeSpeed = 3;
	sptr->FadeToBlack = 5;
	sptr->x = (rnd & 7) + x - 3;
	sptr->y = ((rnd >> 3) & 7) + y - 3;
	sptr->z = ((rnd >> 6) & 7) + z - 3;
	sptr->Xvel = (short)(((rnd >> 2) & 0xFF) + xvel - 128);
	sptr->Yvel = (short)(((rnd >> 4) & 0xFF) + yvel - 128);
	sptr->Zvel = (short)(((rnd >> 6) & 0xFF) + zvel - 128);
	sptr->TransType = 2;
	sptr->Friction = 34;
	sptr->Scalar = 1;
	sptr->Flags = 2;
	sptr->sSize = ((rnd >> 9) & 3) + 4;
	sptr->Size = sptr->sSize;
	sptr->dSize = ((rnd >> 12) & 1) + 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;

	if(smoke) {
		rnd = GetRandomDraw();
		smokeSpark = GetFreeSpark();
		smokeSpark->On = 1;
		smokeSpark->sR = sptr->dR >> 1;
		smokeSpark->sG = sptr->dG >> 1;
		smokeSpark->sB = sptr->dB >> 1;
		smokeSpark->dR = 32;
		smokeSpark->dG = 32;
		smokeSpark->dB = 32;
		smokeSpark->FadeToBlack = 4;
		smokeSpark->TransType = 2;
		smokeSpark->ColFadeSpeed = (rnd & 3) + 8;
		smokeSpark->Life = ((rnd >> 3) & 7) + 13;
		smokeSpark->sLife = smokeSpark->Life;
		smokeSpark->Friction = 4;
		smokeSpark->x = x + (xvel >> 5);
		smokeSpark->y = y + (yvel >> 5);
		smokeSpark->z = z + (zvel >> 5);
		smokeSpark->Xvel = (short)((rnd & 0x3F) + xvel - 32);
		smokeSpark->Yvel = (short)yvel;
		smokeSpark->Zvel = (short)(((rnd >> 6) & 0x3F) + zvel - 32);

		if(rnd & 1) {
			smokeSpark->Flags = 538;
			smokeSpark->RotAng = (short)(rnd >> 3);

			if(rnd & 2) {
				smokeSpark->RotAdd = -16 - (rnd & 0xF);
			} else {
				smokeSpark->RotAdd = (rnd & 0xF) + 16;
			}
		} else {
			smokeSpark->Flags = 522;
		}

		smokeSpark->Gravity = -8 - ((rnd >> 3) & 3);
		smokeSpark->Scalar = 2;
		smokeSpark->MaxYvel = -4 - ((rnd >> 6) & 3);
		smokeSpark->dSize = ((rnd >> 8) & 0xF) + 24;
		smokeSpark->sSize = smokeSpark->dSize >> 3;
		smokeSpark->Size = smokeSpark->dSize >> 3;
	}
}

void TriggerDynamic(long x, long y, long z, long falloff, long r, long g, long b) {
	DYNAMIC* dl;

	if(number_dynamics == MAX_DYNAMICS || !falloff) {
		return;
	}

	dl = &dynamics[number_dynamics];
	dl->on = 1;
	dl->pos.x = x;
	dl->pos.y = y;
	dl->pos.z = z;
	dl->falloff = (unsigned short)(falloff << 8);

	if(falloff < 8) {
		dl->r = (unsigned char)((r * falloff) >> 3);
		dl->g = (unsigned char)((g * falloff) >> 3);
		dl->b = (unsigned char)((b * falloff) >> 3);
	} else {
		dl->r = (unsigned char)r;
		dl->g = (unsigned char)g;
		dl->b = (unsigned char)b;
	}

	number_dynamics++;
}

void TriggerDynamic_MIRROR(long x, long y, long z, long falloff, long r, long g, long b) {
	DYNAMIC* dl;

	for(int i = 0; i < 2; i++) {
		if(number_dynamics == MAX_DYNAMICS || !falloff) {
			return;
		}

		dl = &dynamics[number_dynamics];
		dl->on = 1;
		dl->pos.x = x;
		dl->pos.y = y;
		dl->pos.z = z;
		dl->falloff = (unsigned short)(falloff << 8);

		if(falloff < 8) {
			dl->r = (unsigned char)((r * falloff) >> 3);
			dl->g = (unsigned char)((g * falloff) >> 3);
			dl->b = (unsigned char)((b * falloff) >> 3);
		} else {
			dl->r = (unsigned char)r;
			dl->g = (unsigned char)g;
			dl->b = (unsigned char)b;
		}

		number_dynamics++;
		z = 2 * gfMirrorZPlane - z;
	}
}

void ClearDynamics() {
	number_dynamics = 0;

	for(int i = 0; i < MAX_DYNAMICS; i++) {
		dynamics[i].on = 0;
	}
}

void ControlEnemyMissile(short fx_number) {
	FX_INFO* fx;
	FLOOR_INFO* floor;
	height_types ht;
	long tiltxoff, tiltzoff, OnObject;
	long speed, ox, oy, oz, h, c;
	short room_number, max_speed, max_turn;
	short angles[2];

	fx = GetEffect(currentLevel, fx_number);
	phd_GetVectorAngles(lara_item->pos.pos.x - fx->pos.pos.x, lara_item->pos.pos.y - fx->pos.pos.y - 256, lara_item->pos.pos.z - fx->pos.pos.z, angles);

	if(fx->flag1 == 1) {
		max_turn = 512;
		max_speed = 256;
	} else if(fx->flag1 == 6) {
		if(fx->counter) {
			fx->counter--;
		}

		max_turn = 768;
		max_speed = 192;
	} else {
		max_turn = 768;
		max_speed = 192;
	}

	if(fx->speed < max_speed) {
		if(fx->flag1 == 6) {
			fx->speed++;
		} else {
			fx->speed += 3;
		}

		oy = (unsigned short)angles[0] - (unsigned short)fx->pos.y_rot;

		if(abs(oy) > 0x8000) {
			oy = (unsigned short)fx->pos.y_rot - (unsigned short)angles[0];
		}

		ox = (unsigned short)angles[1] - (unsigned short)fx->pos.x_rot;

		if(abs(ox) > 0x8000) {
			ox = (unsigned short)fx->pos.x_rot - (unsigned short)angles[1];
		}

		oy >>= 3;
		ox >>= 3;

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

		fx->pos.x_rot += (short)ox;

		if(fx->flag1 != 4 && (fx->flag1 != 6 || !fx->counter)) {
			fx->pos.y_rot += (short)oy;
		}
	}

	fx->pos.z_rot += fx->speed << 4;

	if(fx->flag1 == 6) {
		fx->pos.z_rot += fx->speed << 4;
	}

	ox = fx->pos.pos.x;
	oy = fx->pos.pos.y;
	oz = fx->pos.pos.z;
	speed = fx->speed * phd_cos(fx->pos.x_rot) >> W2V_SHIFT;
	fx->pos.pos.x += speed * phd_sin(fx->pos.y_rot) >> W2V_SHIFT;
	fx->pos.pos.y += fx->speed * phd_sin(-fx->pos.x_rot) >> W2V_SHIFT;
	fx->pos.pos.z += speed * phd_cos(fx->pos.y_rot) >> W2V_SHIFT;
	room_number = fx->room_number;
	floor = GetFloor(fx->pos.pos.x, fx->pos.pos.y, fx->pos.pos.z, &room_number);
	h = GetHeight(floor, fx->pos.pos.x, fx->pos.pos.y, fx->pos.pos.z, &ht, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, fx->pos.pos.x, fx->pos.pos.y, fx->pos.pos.z);

	if(fx->pos.pos.y >= h || fx->pos.pos.y <= c) {
		fx->pos.pos.x = ox;
		fx->pos.pos.y = oy;
		fx->pos.pos.z = oz;

		if(fx->flag1 != 6) {
			ExplodeFX(fx, 0, -32);
		}

		if(fx->flag1 == 1) {
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x18008040, (((~GetRoom(currentLevel, fx->room_number)->flags & 0xFF) >> 4) & 2) << 16); // decipher me
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 2, fx->room_number);
		} else if(fx->flag1 == 0) {
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10008040, 0);
		} else if(fx->flag1 == 3 || fx->flag1 == 4) {
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10004080, 0);
		} else if(fx->flag1 == 5) {
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10806000, 0);
		} else if(fx->flag1 == 2) {
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10808000, 0);
		} else if(fx->flag1 == 6) {
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 0, fx->room_number);
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 64, 0x18806000, 0x20000);
			fx->pos.pos.y -= 128;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x10807000, 0x20000);
			fx->pos.pos.y += 256;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x10807000, 0x20000);
		}

		KillEffect(fx_number);
		return;
	}

	if(ItemNearLara(&fx->pos, 200)) {
		lara_item->hit_status = 1;

		if(fx->flag1 != 6) {
			ExplodeFX(fx, 0, -32);
		}

		KillEffect(fx_number);

		if(fx->flag1 == 1) {
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 64, 0x18008040, 0);
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 2, fx->room_number);
			LaraBurn();
			lara.BurnGreen = 1;
		} else if(fx->flag1 == 0) {
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0x580018, 48, 0x10008040, (((~GetRoom(currentLevel, fx->room_number)->flags & 0xFF) >> 4) & 2) << 16);
		} else if(fx->flag1 == 3 || fx->flag1 == 4) {
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10004080, 0x10000);
		} else if(fx->flag1 == 5) {
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10806000, 0x20000);
		} else if(fx->flag1 == 2) {
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xA00020, 64, 0x10808000, 0x20000);
		} else if(fx->flag1 == 6) {
			TriggerExplosionSparks(ox, oy, oz, 3, -2, 0, fx->room_number);
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 64, 0x18806000, 0);
			fx->pos.pos.y -= 128;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x10807000, 0);
			fx->pos.pos.y += 256;
			TriggerShockwave((PHD_VECTOR*)&fx->pos, 0xF00030, 48, 0x10807000, 0);
			LaraBurn();
		}
	} else {
		if(room_number != fx->room_number) {
			EffectNewRoom(fx_number, room_number);
		}

		ox -= fx->pos.pos.x;
		oy -= fx->pos.pos.y;
		oz -= fx->pos.pos.z;

		if(wibble & 4 || fx->flag1 == 1 || fx->flag1 == 5 || fx->flag1 == 2) {
			if(fx->flag1 == 0) {
				TriggerSethMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
			} else if(fx->flag1 == 1) {
				TriggerSethMissileFlame(fx_number, ox << 5, oy << 5, oz << 5);
			} else if(fx->flag1 == 3 || fx->flag1 == 4 || fx->flag1 == 5) {
				TriggerDemigodMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
			} else if(fx->flag1 == 2) {
				TriggerHarpyMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
			} else if(fx->flag1 == 6) {
				TriggerCrocgodMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
			}
		}
	}
}

RIPPLE_STRUCT* GetFreeRipple() {
	for(int i = 0; i < nRipples; ++i) {
		if(!(ripples[i].flags & 1)) {
			return &ripples[i];
		}
	}
	size_t idx = nRipples;
	nRipples = (nRipples * 2) + 4;
	ripples = realloc(ripples, nRipples * sizeof(RIPPLE_STRUCT));
	for(int i = idx; i < nRipples; ++i) {
		RIPPLE_STRUCT* bptr = &ripples[i];
		*bptr = (RIPPLE_STRUCT){ 0 };
	}
	return &ripples[idx];
}

void SetupRipple(long x, long y, long z, long size, long flags) {
	RIPPLE_STRUCT* ripple;

	ripple = GetFreeRipple();
	ripple->flags = (char)flags | 1;
	ripple->size = (unsigned char)size;
	ripple->life = (GetRandomControl() & 0x3F) + 64;
	ripple->init = 1;
	ripple->x = x;
	ripple->y = y;
	ripple->z = z;
	ripple->x += (GetRandomControl() & 0x3F) - 32;
	ripple->z += (GetRandomControl() & 0x3F) - 32;
}



void TriggerWaterfallMist(long x, long y, long z, long ang) {
	SPARKS* sptr;
	long offsets[4];
	long ang2, ps, pc, rad;
	short vs, vc;

	offsets[0] = 576;
	offsets[1] = 203;
	offsets[2] = -203;
	offsets[3] = -576;
	ang2 = (ang + 1024) & 0xFFF;
	ps = rcossin_tbl[ang2 << 2];
	pc = rcossin_tbl[(ang2 << 2) + 1];
	vs = rcossin_tbl[ang << 2];
	vc = rcossin_tbl[(ang << 2) + 1];

	for(int i = 0; i < 4; i++) {
		sptr = GetFreeSpark();
		sptr->On = 1;
		sptr->sR = 128;
		sptr->sG = 128;
		sptr->sB = 128;
		sptr->dR = 192;
		sptr->dG = 192;
		sptr->dB = 192;
		sptr->ColFadeSpeed = 2;
		sptr->FadeToBlack = 4;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 6;
		sptr->sLife = sptr->Life;
		rad = (GetRandomControl() & 0x1F) + offsets[i] - 16;
		sptr->x = ((rad * ps) >> 12) + x + (GetRandomControl() & 0xF) - 8;
		sptr->y = (GetRandomControl() & 0xF) + y - 8;
		sptr->z = ((rad * pc) >> 12) + z + (GetRandomControl() & 0xF) - 8;
		sptr->Xvel = vs >> 12;
		sptr->Yvel = 0;
		sptr->Zvel = vc >> 12;
		sptr->Friction = 3;
		if(GetRandomControl() & 1) {
			sptr->Flags = 538;
			sptr->RotAng = GetRandomControl() & 0xFFF;

			if(GetRandomControl() & 1) {
				sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
			} else {
				sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
			}
		} else {
			sptr->Flags = 522;
		}

		sptr->Scalar = 6;
		sptr->Gravity = 0;
		sptr->MaxYvel = 0;
		sptr->dSize = (GetRandomControl() & 7) + 12;
		sptr->sSize = sptr->dSize >> 1;
		sptr->Size = sptr->dSize >> 1;
	}
}

void TriggerDartSmoke(long x, long y, long z, long xv, long zv, long hit) {
	SPARKS* sptr;
	long dx, dz, rand;

	dx = lara_item->pos.pos.x - x;
	dz = lara_item->pos.pos.z - z;

	if(dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000) {
		return;
	}

	sptr = GetFreeSpark();
	sptr->On = 1;
	sptr->sR = 16;
	sptr->sG = 8;
	sptr->sB = 4;
	sptr->dR = 64;
	sptr->dG = 48;
	sptr->dB = 32;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 4;
	sptr->Life = (GetRandomControl() & 3) + 32;
	sptr->sLife = sptr->Life;
	sptr->TransType = 2;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;

	if(hit) {
		sptr->Xvel = (short)((GetRandomControl() & 0xFF) - xv - 128);
		sptr->Yvel = -4 - (GetRandomControl() & 3);
		sptr->Zvel = (short)((GetRandomControl() & 0xFF) - zv - 128);
	} else {
		if(xv) {
			sptr->Xvel = (short)-xv;
		} else {
			sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
		}

		sptr->Yvel = -4 - (GetRandomControl() & 3);

		if(zv) {
			sptr->Zvel = (short)-zv;
		} else {
			sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
		}
	}

	sptr->Friction = 3;

	if(GetRandomControl() & 1) {
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if(GetRandomControl() & 1) {
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		} else {
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
		}
	} else {
		sptr->Flags = 522;
	}

	sptr->Scalar = 1;
	rand = (GetRandomControl() & 0x3F) + 72;

	if(hit) {
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
		sptr->Size = (unsigned char)(rand >> 3);
		sptr->sSize = sptr->Size;
		sptr->dSize = (unsigned char)(rand >> 1);
	} else {
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
		sptr->Gravity = -4 - (GetRandomControl() & 3);
		sptr->Size = (unsigned char)(rand >> 4);
		sptr->sSize = sptr->Size;
		sptr->dSize = (unsigned char)rand;
	}
}

void KillAllCurrentItems(short item_number) {
	KillEverythingFlag = 1;
}

void KillEverything() {
	KillEverythingFlag = 0;
}

void TriggerExplosionBubble(long x, long y, long z, short room_number) {
	SPARKS* sptr;
	PHD_3DPOS pos;
	long dx, dz;
	unsigned char size;

	dx = lara_item->pos.pos.x - x;
	dz = lara_item->pos.pos.z - z;

	if(dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000) {
		return;
	}

	sptr = GetFreeSpark();
	sptr->On = 1;
	sptr->sR = 128;
	sptr->sG = 64;
	sptr->sB = 0;
	sptr->dR = 128;
	sptr->dG = 128;
	sptr->dB = 128;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 12;
	sptr->Life = 24;
	sptr->sLife = 24;
	sptr->TransType = 2;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Friction = 0;
	sptr->Flags = 2058;
	sptr->Scalar = 3;
	sptr->Def = GetObjectInfo(currentLevel, DEFAULT_SPRITES)->mesh_index + 13;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	size = (GetRandomControl() & 7) + 63;
	sptr->Size = size >> 1;
	sptr->sSize = size >> 1;
	sptr->dSize = size << 1;

	for(int i = 0; i < 7; i++) {
		pos.pos.x = (GetRandomControl() & 0x1FF) + x - 256;
		pos.pos.y = (GetRandomControl() & 0x7F) + y - 64;
		pos.pos.z = (GetRandomControl() & 0x1FF) + z - 256;
		CreateBubble(&pos, room_number, 6, 15);
	}
}

void ControlColouredLights(short item_number) {
	ITEM_INFO* item;
	long objnum;
	unsigned char colours[5][3] = {
		{ 255, 0, 0 }, // RED_LIGHT
		{ 0, 255, 0 }, // GREEN_LIGHT
		{ 0, 0, 255 }, // BLUE_LIGHT
		{ 255, 192, 0 }, // unused
		{ 224, 224, 255 } // unused
	};

	item = GetItem(currentLevel, item_number);

	if(TriggerActive(item)) {
		objnum = item->object_number - RED_LIGHT;
		TriggerDynamic(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, 24, colours[objnum][0], colours[objnum][1], colours[objnum][2]);
	}
}

void DetatchSpark(long num, long type) {
	SPARKS* sptr;
	FX_INFO* fx;
	ITEM_INFO* item;

	for(int i = 0; i < nSpark; i++) {
		sptr = &spark[i];
		if(sptr->On && (sptr->Flags & type) && sptr->FxObj == num) {

			if(type == 64) {
				fx = GetEffect(currentLevel, num);
				sptr->x += fx->pos.pos.x;
				sptr->y += fx->pos.pos.y;
				sptr->z += fx->pos.pos.z;
				sptr->Flags &= ~64;
			} else if(type == 128) {
				item = GetItem(currentLevel, num);
				sptr->x += item->pos.pos.x;
				sptr->y += item->pos.pos.y;
				sptr->z += item->pos.pos.z;
				sptr->Flags &= ~128;
			}
		}
	}
}

SPARKS* GetFreeSpark() {
	SPARKS* newBuffer = NULL;
	size_t oldSize = nSpark;
	for(int i = 0; i < nSpark; ++i) {
		if(!spark[i].On) {
			spark[i] = (SPARKS){ 0 };
			spark[i].Dynamic = -1;
			spark[i].Def = (unsigned char)GetObjectInfo(currentLevel, DEFAULT_SPRITES)->mesh_index;
			return &spark[i];
		}
	}
	size_t idx = nSpark;
	nSpark = (nSpark * 2) + 4;
	newBuffer = realloc(spark, nSpark * sizeof(SPARKS));
	if(!newBuffer) {
		nSpark = oldSize;
		return spark;
	}
	spark = newBuffer;
	for(int i = idx; i < nSpark; ++i) {
		SPARKS* sptr = &spark[i];
		*sptr = (SPARKS){ 0 };
		sptr->Dynamic = -1;
		sptr->Def = (unsigned char)GetObjectInfo(currentLevel, DEFAULT_SPRITES)->mesh_index;
	}
	return &spark[idx];
}

void UpdateSparks() {
	SPARKS* sptr;
	SP_DYNAMIC* dynamic;
	short bounds[6];
	long fade, uw, rad, rnd, x, y, z, r, g, b, falloff;

	GetBoundsAccurate(lara_item, bounds);
	DeadlyBounds[0] = lara_item->pos.pos.x + bounds[0];
	DeadlyBounds[1] = lara_item->pos.pos.x + bounds[1];
	DeadlyBounds[2] = lara_item->pos.pos.y + bounds[2];
	DeadlyBounds[3] = lara_item->pos.pos.y + bounds[3];
	DeadlyBounds[4] = lara_item->pos.pos.z + bounds[4];
	DeadlyBounds[5] = lara_item->pos.pos.z + bounds[5];

	for(int i = 0; i < nSpark; i++) {

		if(!spark[i].On) {
			continue;
		}

		spark[i].Life--;

		if(!spark[i].Life) {
			if(spark[i].Dynamic != -1) {
				spark_dynamics[spark[i].Dynamic].On = 0;
			}

			spark[i].On = 0;
			continue;
		}

		if(spark[i].sLife - spark[i].Life < spark[i].ColFadeSpeed) {
			fade = ((spark[i].sLife - spark[i].Life) << 16) / spark[i].ColFadeSpeed;
			spark[i].R = (unsigned char)(spark[i].sR + ((fade * (spark[i].dR - spark[i].sR)) >> 16));
			spark[i].G = (unsigned char)(spark[i].sG + ((fade * (spark[i].dG - spark[i].sG)) >> 16));
			spark[i].B = (unsigned char)(spark[i].sB + ((fade * (spark[i].dB - spark[i].sB)) >> 16));
		} else if(spark[i].Life < spark[i].FadeToBlack) {
			fade = ((spark[i].Life - spark[i].FadeToBlack) << 16) / spark[i].FadeToBlack + 0x10000;
			spark[i].R = (unsigned char)((spark[i].dR * fade) >> 16);
			spark[i].G = (unsigned char)((spark[i].dG * fade) >> 16);
			spark[i].B = (unsigned char)((spark[i].dB * fade) >> 16);

			if(spark[i].R < 8 && spark[i].G < 8 && spark[i].B < 8) {
				spark[i].On = 0;
				continue;
			}
		} else {
			spark[i].R = spark[i].dR;
			spark[i].G = spark[i].dG;
			spark[i].B = spark[i].dB;
		}

		if(spark[i].Life == spark[i].FadeToBlack && spark[i].Flags & 0x800) {
			spark[i].dSize >>= 2;
		}

		if(spark[i].Flags & 0x10) {
			spark[i].RotAng = (spark[i].RotAng + spark[i].RotAdd) & 0xFFF;
		}

		if(spark[i].sLife - spark[i].Life == spark[i].extras >> 3 && spark[i].extras & 7) {
			if(spark[i].Flags & 0x800) {
				uw = 1;
			} else if(spark[i].Flags & 0x2000) {
				uw = 2;
			} else {
				uw = 0;
			}

			for(int j = 0; j < (spark[i].extras & 7); j++) {
				TriggerExplosionSparks(spark[i].x, spark[i].y, spark[i].z, (spark[i].extras & 7) - 1, spark[i].Dynamic, uw, spark[i].RoomNumber);
				spark[i].Dynamic = -1;
			}

			if(spark[i].Flags & 0x800) {
				TriggerExplosionBubble(spark[i].x, spark[i].y, spark[i].z, spark[i].RoomNumber);
			}

			spark[i].extras = 0;
		}

		fade = ((spark[i].sLife - spark[i].Life) << 16) / spark[i].sLife;
		spark[i].Yvel += spark[i].Gravity;

		if(spark[i].MaxYvel) {
			if(spark[i].Yvel < 0 && spark[i].Yvel < spark[i].MaxYvel << 5 || spark[i].Yvel > 0 && spark[i].Yvel > spark[i].MaxYvel << 5) {
				spark[i].Yvel = spark[i].MaxYvel << 5;
			}
		}

		if(spark[i].Friction & 0xF) {
			spark[i].Xvel -= spark[i].Xvel >> (spark[i].Friction & 0xF);
			spark[i].Zvel -= spark[i].Zvel >> (spark[i].Friction & 0xF);
		}

		if(spark[i].Friction & 0xF0) {
			spark[i].Yvel -= spark[i].Yvel >> (spark[i].Friction >> 4);
		}

		spark[i].x += spark[i].Xvel >> 5;
		spark[i].y += spark[i].Yvel >> 5;
		spark[i].z += spark[i].Zvel >> 5;

		if(spark[i].Flags & 0x100) {
			spark[i].x += SmokeWindX >> 1;
			spark[i].z += SmokeWindZ >> 1;
		}

		spark[i].Size = (unsigned char)(spark[i].sSize + ((fade * (spark[i].dSize - spark[i].sSize)) >> 16));

		if(spark[i].Flags & 1 && !lara.burn || spark[i].Flags & 0x400) {
			rad = spark[i].Size << spark[i].Scalar >> 1;

			if(spark[i].x + rad > DeadlyBounds[0] && spark[i].x - rad < DeadlyBounds[1] && spark[i].y + rad > DeadlyBounds[2] && spark[i].y - rad < DeadlyBounds[3] && spark[i].z + rad > DeadlyBounds[4] && spark[i].z - rad < DeadlyBounds[5]) {
				if(spark[i].Flags & 1) {
					LaraBurn();
				} else {
					lara_item->hit_points -= 2;
				}
			}
		}
	}

	for(int i = 0; i < nSpark; i++) {
		sptr = &spark[i];

		if(!sptr->On || sptr->Dynamic == -1) {
			continue;
		}

		dynamic = &spark_dynamics[sptr->Dynamic];

		if(dynamic->Flags & 3) {
			rnd = GetRandomControl();
			x = sptr->x + 16 * (rnd & 0xF);
			y = sptr->y + (rnd & 0xF0);
			z = sptr->z + ((rnd >> 4) & 0xF0);
			falloff = sptr->sLife - sptr->Life - 1;

			if(falloff < 2) {
				if(dynamic->Falloff < 28) {
					dynamic->Falloff += 6;
				}

				r = 255 - (falloff << 3) - (rnd & 0x1F);
				g = 255 - (falloff << 4) - (rnd & 0x1F);
				b = 255 - (falloff << 6) - (rnd & 0x1F);
			} else if(falloff < 4) {
				if(dynamic->Falloff < 28) {
					dynamic->Falloff += 6;
				}

				r = 255 - (falloff << 3) - (rnd & 0x1F);
				g = 128 - (falloff << 3);
				b = 128 - (falloff << 5);

				if(b < 0) {
					b = 0;
				}
			} else {
				if(dynamic->Falloff > 0) {
					dynamic->Falloff--;
				}

				r = 224 + (rnd & 0x1F);
				g = 128 + ((rnd >> 4) & 0x1F);
				b = (rnd >> 8) & 0x3F;
			}

			if(sptr->Flags & 0x2000) {
				TriggerDynamic(x, y, z, dynamic->Falloff > 31 ? 31 : dynamic->Falloff, b, r, g);
			} else {
				TriggerDynamic(x, y, z, dynamic->Falloff > 31 ? 31 : dynamic->Falloff, r, g, b);
			}
		}
	}
}

void TriggerRicochetSpark(GAME_VECTOR* pos, long ang, long num, long smoke_only) {
	SPARKS* sptr;
	long rnd;

	if(!smoke_only) {
		for(int i = 0; i < num; i++) {
			sptr = GetFreeSpark();
			rnd = GetRandomControl();
			sptr->On = 1;
			sptr->sR = 128;
			sptr->sG = (rnd & 0xF) + 16;
			sptr->sB = 0;
			sptr->dR = 96;
			sptr->dG = ((rnd >> 4) & 0x1F) + 48;
			sptr->dB = 0;
			sptr->ColFadeSpeed = 2;
			sptr->FadeToBlack = 4;
			sptr->Life = 9;
			sptr->sLife = 9;
			sptr->TransType = 2;
			sptr->x = pos->pos.x;
			sptr->y = pos->pos.y;
			sptr->z = pos->pos.z;
			sptr->Yvel = (rnd & 0xFFF) - 2048;
			sptr->Gravity = (rnd >> 7) & 0x1F;
			rnd = (((rnd >> 3) & 0x7FF) + ang - 1024) & 0xFFF;
			sptr->Zvel = rcossin_tbl[(rnd << 1) + 1] >> 2;
			sptr->Xvel = -rcossin_tbl[rnd << 1] >> 2;
			sptr->Friction = 34;
			sptr->Flags = 0;
			sptr->MaxYvel = 0;
		}

		rnd = GetRandomControl();
		sptr = GetFreeSpark();
		sptr->On = 1;
		sptr->sR = 48;
		sptr->sG = (rnd & 0xF) + 32;
		sptr->sB = 0;
		sptr->dR = 0;
		sptr->dG = 0;
		sptr->dB = 0;
		sptr->ColFadeSpeed = 4;
		sptr->FadeToBlack = 0;
		sptr->Life = 4;
		sptr->sLife = 4;
		sptr->TransType = 2;
		sptr->x = pos->pos.x;
		sptr->y = pos->pos.y;
		sptr->z = pos->pos.z;
		sptr->Xvel = 0;
		sptr->Yvel = 0;
		sptr->Zvel = 0;
		sptr->Flags = 26;
		sptr->RotAng = (rnd >> 2) & 0xFFF;

		if(rnd & 1) {
			sptr->RotAdd = -64 - ((rnd >> 1) & 0x3F);
		} else {
			sptr->RotAdd = ((rnd >> 1) & 0x3F) + 64;
		}

		sptr->Scalar = 3;
		sptr->Def = GetObjectInfo(currentLevel, DEFAULT_SPRITES)->mesh_index + 12;
		sptr->Size = ((rnd >> 10) & 7) + 8;
		sptr->sSize = sptr->Size;
		sptr->dSize = 1;
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
	}

	for(int i = 0; i < 1 - smoke_only; i++) {
		rnd = GetRandomControl();
		sptr = GetFreeSpark();
		sptr->On = 1;
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 40;
		sptr->dG = 40;
		sptr->dB = 48;
		sptr->ColFadeSpeed = (rnd & 3) + 4;
		sptr->FadeToBlack = 8;
		sptr->Life = ((rnd >> 2) & 7) + 16;
		sptr->sLife = sptr->Life;
		sptr->x = pos->pos.x;
		sptr->y = pos->pos.y;
		sptr->z = pos->pos.z;

		if(smoke_only) {
			sptr->ColFadeSpeed >>= 1;
			sptr->FadeToBlack = 4;
			sptr->Life >>= 1;
			sptr->sLife >>= 1;
			sptr->Xvel = (rnd & 0x1FF) - 256;
			sptr->Yvel = ((rnd >> 2) & 0x1FF) - 256;
			sptr->Zvel = ((rnd >> 4) & 0x1FF) - 256;
		} else {
			sptr->Yvel = 0;
			sptr->Xvel = 0;
			sptr->Zvel = 0;
		}

		sptr->TransType = 2;
		sptr->Friction = 0;
		sptr->Flags = 26;
		sptr->RotAng = (short)(rnd >> 3);

		if(rnd & 1) {
			sptr->RotAdd = -16 - (rnd & 0xF);
		} else {
			sptr->RotAdd = (rnd & 0xF) + 16;
		}

		sptr->Scalar = 2;
		sptr->Gravity = -4 - ((rnd >> 9) & 3);
		sptr->MaxYvel = -4 - ((rnd >> 6) & 3);
		sptr->Size = ((rnd >> 5) & 7) + 4;
		sptr->sSize = sptr->Size;
		sptr->dSize = sptr->Size << 2;
	}
}

void TriggerExplosionSparks(long x, long y, long z, long extras, long dynamic, long uw, short room_number) {
	SPARKS* sptr;
	SP_DYNAMIC* pDL;
	long dx, dz, scalar, mirror, i;
	unsigned char extras_table[4];
	unsigned char r, g, b;

	extras_table[0] = 0;
	extras_table[1] = 4;
	extras_table[2] = 7;
	extras_table[3] = 10;
	dx = lara_item->pos.pos.x - x;
	dz = lara_item->pos.pos.z - z;
	scalar = 0;
	mirror = 0;

	if(dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000) {
		return;
	}

	if(room_number < 0) {
		room_number = -room_number;
		scalar = 1;
	}

	if(room_number == gfMirrorRoom && gfLevelFlags & GF_MIRROR) {
		mirror = 1;
	}

	do {
		sptr = GetFreeSpark();
		sptr->On = 1;
		sptr->sR = 255;

		if(uw == 1) {
			sptr->sG = (GetRandomControl() & 0x3F) + 128;
			sptr->sB = 32;
			sptr->dR = 192;
			sptr->dG = (GetRandomControl() & 0x1F) + 64;
			sptr->dB = 0;
			sptr->ColFadeSpeed = 7;
			sptr->FadeToBlack = 8;
			sptr->TransType = 2;
			sptr->Life = (GetRandomControl() & 7) + 16;
			sptr->sLife = sptr->Life;
			sptr->RoomNumber = (unsigned char)room_number;
		} else {
			sptr->sG = (GetRandomControl() & 0xF) + 32;
			sptr->sB = 0;
			sptr->dR = (GetRandomControl() & 0x3F) + 192;
			sptr->dG = (GetRandomControl() & 0x3F) + 128;
			sptr->dB = 32;
			sptr->ColFadeSpeed = 8;
			sptr->FadeToBlack = 16;
			sptr->TransType = 2;
			sptr->Life = (GetRandomControl() & 7) + 24;
			sptr->sLife = sptr->Life;
		}

		sptr->extras = (unsigned char)(extras | ((extras_table[extras] + (GetRandomControl() & 7) + 28) << 3));
		sptr->Dynamic = (char)dynamic;

		if(dynamic == -2) {
			for(i = 0; i < 8; i++) {
				pDL = &spark_dynamics[i];

				if(!pDL->On) {
					pDL->On = 1;
					pDL->Falloff = 4;

					if(uw == 1) {
						pDL->Flags = 2;
					} else {
						pDL->Flags = 1;
					}

					sptr->Dynamic = (char)i;
					break;
				}
			}

			if(i == 8) {
				sptr->Dynamic = -1;
			}
		}

		sptr->x = (GetRandomControl() & 0x1F) + x - 16;
		sptr->y = (GetRandomControl() & 0x1F) + y - 16;
		sptr->z = (GetRandomControl() & 0x1F) + z - 16;
		sptr->Xvel = (GetRandomControl() & 0xFFF) - 2048;
		sptr->Yvel = (GetRandomControl() & 0xFFF) - 2048;
		sptr->Zvel = (GetRandomControl() & 0xFFF) - 2048;

		if(dynamic != -2 || uw == 1) {
			sptr->x = (GetRandomControl() & 0x1F) + x - 16;
			sptr->y = (GetRandomControl() & 0x1F) + y - 16;
			sptr->z = (GetRandomControl() & 0x1F) + z - 16;
		} else {
			sptr->x = (GetRandomControl() & 0x1FF) + x - 256;
			sptr->y = (GetRandomControl() & 0x1FF) + y - 256;
			sptr->z = (GetRandomControl() & 0x1FF) + z - 256;
		}

		if(uw == 1) {
			sptr->Friction = 17;
		} else {
			sptr->Friction = 51;
		}

		if(GetRandomControl() & 1) {
			if(uw == 1) {
				sptr->Flags = 2586;
			} else {
				sptr->Flags = 538;
			}

			sptr->RotAng = GetRandomControl() & 0xFFF;
			sptr->RotAdd = (GetRandomControl() & 0xFF) + 128;
		} else if(uw == 1) {
			sptr->Flags = 2570;
		} else {
			sptr->Flags = 522;
		}

		sptr->Scalar = 3;
		sptr->Gravity = 0;
		sptr->Size = (GetRandomControl() & 0xF) + 40;
		sptr->sSize = sptr->Size << scalar;
		sptr->dSize = sptr->Size << (scalar + 1);
		sptr->Size <<= scalar;
		GetRandomControl();
		sptr->MaxYvel = 0;

		if(uw == 2) {
			r = sptr->sR;
			g = sptr->sG;
			b = sptr->sB;
			sptr->sR = b;
			sptr->sG = r;
			sptr->sB = g;

			r = sptr->dR;
			g = sptr->dG;
			b = sptr->dB;
			sptr->dR = b;
			sptr->dG = r;
			sptr->dB = g;

			sptr->Flags |= 0x2000;
		} else if(extras) {
			TriggerExplosionSmoke(x, y, z, uw);
		} else {
			TriggerExplosionSmokeEnd(x, y, z, uw);
		}

		z = 2 * gfMirrorZPlane - z;
		mirror--;
	} while(mirror >= 0);
}

void TriggerFireFlame(long x, long y, long z, long body_part, long type) {
	SPARKS* sptr;
	long dx, dz, size;

	dx = lara_item->pos.pos.x - x;
	dz = lara_item->pos.pos.z - z;

	if(dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000) {
		return;
	}

	sptr = GetFreeSpark();
	sptr->On = 1;

	if(type == 2) {
		sptr->sR = (GetRandomControl() & 0x1F) + 48;
		sptr->sG = sptr->sR;
		sptr->sB = (GetRandomControl() & 0x3F) + 192;
	} else if(type == 254) {
		sptr->sR = 48;
		sptr->sG = 255;
		sptr->sB = (GetRandomControl() & 0x1F) + 48;
		sptr->dR = 32;
		sptr->dG = (GetRandomControl() & 0x3F) + 192;
		sptr->dB = (GetRandomControl() & 0x3F) + 128;
	} else {
		sptr->sR = 255;
		sptr->sG = (GetRandomControl() & 0x1F) + 48;
		sptr->sB = 48;
	}

	if(type != 254) {
		sptr->dR = (GetRandomControl() & 0x3F) + 192;
		sptr->dG = (GetRandomControl() & 0x3F) + 128;
		sptr->dB = 32;
	}

	if(body_part == -1) {
		sptr->FadeToBlack = 16;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
		sptr->Life = (GetRandomControl() & 3) + 28;
	} else if(type == 2 || type == 255 || type == 254) {
		sptr->FadeToBlack = 6;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 5;
		sptr->Life = (type < 254 ? 0 : 8) + (GetRandomControl() & 3) + 16;
	} else {
		sptr->FadeToBlack = 8;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 20;
		sptr->Life = (GetRandomControl() & 7) + 40;
	}

	sptr->sLife = sptr->Life;
	sptr->TransType = 2;

	if(body_part != -1) {
		sptr->x = (GetRandomControl() & 0x1F) - 16;
		sptr->y = 0;
		sptr->z = (GetRandomControl() & 0x1F) - 16;
	} else if(!type || type == 1) {
		sptr->x = (GetRandomControl() & 0x1F) + x - 16;
		sptr->y = y;
		sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	} else if(type < 254) {
		sptr->x = (GetRandomControl() & 0xF) + x - 8;
		sptr->y = y;
		sptr->z = (GetRandomControl() & 0xF) + z - 8;
	} else {
		sptr->x = (GetRandomControl() & 0x3F) + x - 32;
		sptr->y = y;
		sptr->z = (GetRandomControl() & 0x3F) + z - 32;
	}

	if(type == 2) {
		sptr->Xvel = (GetRandomControl() & 0x1F) - 16;
		sptr->Yvel = -1024 - (GetRandomControl() & 0x1FF);
		sptr->Zvel = (GetRandomControl() & 0x1F) - 16;
		sptr->Friction = 68;
	} else {
		sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Yvel = -16 - (GetRandomControl() & 0xF);
		sptr->Zvel = (GetRandomControl() & 0xFF) - 128;

		if(type == 1) {
			sptr->Friction = 51;
		} else {
			sptr->Friction = 5;
		}
	}

	if(GetRandomControl() & 1) {
		if(body_part == -1) {
			sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
			sptr->Flags = 538;
			sptr->MaxYvel = -16 - (GetRandomControl() & 7);
		} else {
			sptr->Flags = 602;
			sptr->FxObj = (unsigned char)body_part;
			sptr->Gravity = -32 - (GetRandomControl() & 0x3F);
			sptr->MaxYvel = -24 - (GetRandomControl() & 7);
		}

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if(GetRandomControl() & 1) {
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		} else {
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
		}
	} else {
		if(body_part == -1) {
			sptr->Flags = 522;
			sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
			sptr->MaxYvel = -16 - (GetRandomControl() & 7);
		} else {
			sptr->Flags = 586;
			sptr->FxObj = (unsigned char)body_part;
			sptr->Gravity = -32 - (GetRandomControl() & 0x3F);
			sptr->MaxYvel = -24 - (GetRandomControl() & 7);
		}
	}

	sptr->Scalar = 2;

	if(!type) {
		size = (GetRandomControl() & 0x1F) + 128;
	} else if(type == 1) {
		size = (GetRandomControl() & 0x1F) + 64;
	} else if(type < 254) {
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
		size = (GetRandomControl() & 0x1F) + 32;
	} else {
		size = (GetRandomControl() & 0xF) + 48;
	}

	sptr->Size = (unsigned char)size;
	sptr->sSize = sptr->Size;

	if(type == 2) {
		sptr->dSize = sptr->Size >> 2;
	} else {
		sptr->dSize = sptr->Size >> 4;

		if(type == 7) {
			sptr->ColFadeSpeed >>= 2;
			sptr->FadeToBlack >>= 2;
			sptr->Life >>= 2;
			sptr->sLife >>= 2;
		}
	}
}

void TriggerSuperJetFlame(ITEM_INFO* item, long yvel, long deadly) {
	SPARKS* sptr;
	long dx, dy, dz;

	dx = lara_item->pos.pos.x - item->pos.pos.x;
	dz = lara_item->pos.pos.z - item->pos.pos.z;

	if(dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000) {
		return;
	}

	dy = (GetRandomControl() & 0x1FF) - yvel;

	if(dy < 512) {
		dy = 512;
	}

	sptr = GetFreeSpark();
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sR;
	sptr->sB = (GetRandomControl() & 0x3F) + 192;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Life = (unsigned char)((dy >> 9) + (GetRandomControl() & 7) + 16);
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) + item->pos.pos.x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + item->pos.pos.y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + item->pos.pos.z - 16;
	sptr->Friction = 51;
	sptr->MaxYvel = 0;

	if(deadly) {
		sptr->Flags = 539;
	} else {
		sptr->Flags = 538;
	}

	sptr->Scalar = 2;
	sptr->dSize = (unsigned char)((GetRandomControl() & 0xF) + (dy >> 6) + 16);
	sptr->sSize = sptr->dSize >> 1;
	sptr->Size = sptr->dSize >> 1;

	if((-item->trigger_flags & 7) == 1) {
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Yvel = (short)-dy;
		sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
		sptr->dSize += sptr->dSize >> 2;
		return;
	}

	sptr->y -= 64;
	sptr->Gravity = (short)(-((dy >> 9) + GetRandomControl() % (dy >> 8)));
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	dy -= dy >> 2;

	if(!item->pos.y_rot) {
		sptr->Zvel = (short)-dy;
	} else if(item->pos.y_rot == 0x4000) {
		sptr->Xvel = (short)-dy;
	} else if(item->pos.y_rot == -0x8000) {
		sptr->Zvel = (short)dy;
	} else {
		sptr->Xvel = (short)dy;
	}
}

void TriggerRocketSmoke(long x, long y, long z, long col) {
	SPARKS* sptr;

	sptr = GetFreeSpark();
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = (unsigned char)(col + 64);
	sptr->dG = (unsigned char)(col + 64);
	sptr->dB = (unsigned char)(col + 64);
	sptr->FadeToBlack = 12;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 20;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -4 - (GetRandomControl() & 3);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 4;

	if(GetRandomControl() & 1) {
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if(GetRandomControl() & 1) {
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		} else {
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
		}
	} else {
		sptr->Flags = 522;
	}

	sptr->Scalar = 3;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	sptr->dSize = (GetRandomControl() & 7) + 32;
	sptr->sSize = sptr->dSize >> 2;
	sptr->Size = sptr->dSize >> 2;
}

SPLASH_STRUCT* GetFreeSplash() {
	for(int i = 0; i < nSplashes; ++i) {
		if(!(splashes[i].flags & 1)) {
			return &splashes[i];
		}
	}
	size_t idx = nSplashes;
	nSplashes = (nSplashes * 2) + 4;
	splashes = realloc(splashes, nSplashes * sizeof(SPLASH_STRUCT));
	for(int i = idx; i < nSplashes; ++i) {
		SPLASH_STRUCT* bptr = &splashes[i];
		*bptr = (SPLASH_STRUCT){ 0 };
	}
	return &splashes[idx];
}

void SetupSplash(SPLASH_SETUP* setup) {
	SPLASH_STRUCT* splash;

	splash = GetFreeSplash();
	splash->flags = 1;
	splash->pos.x = setup->pos.x;
	splash->pos.y = setup->pos.y;
	splash->pos.z = setup->pos.z;
	splash->life = 96;
	splash->InnerRad = setup->InnerRad;
	splash->InnerSize = setup->InnerSize;
	splash->InnerRadVel = setup->InnerRadVel;
	splash->InnerYVel = setup->InnerYVel;
	splash->InnerY = setup->InnerYVel >> 2;
	splash->MiddleRad = setup->pad1;
	splash->MiddleSize = setup->MiddleRad;
	splash->MiddleRadVel = setup->MiddleSize;
	splash->MiddleYVel = setup->MiddleRadVel;
	splash->MiddleY = setup->MiddleRadVel >> 2;
	splash->OuterRad = setup->MiddleYVel;
	splash->OuterSize = setup->pad2;
	splash->OuterRadVel = setup->OuterRad;
	SoundEffect(SFX_LARA_SPLASH, (PHD_3DPOS*)setup, SFX_DEFAULT);
}

void UpdateSplashes() //(and ripples)
{
	SPLASH_STRUCT* splash;
	RIPPLE_STRUCT* ripple;

	for(int i = 0; i < nSplashes; i++) {
		splash = &splashes[i];

		if(!(splash->flags & 1)) {
			continue;
		}

		splash->InnerRad += splash->InnerRadVel >> 5;
		splash->InnerSize += splash->InnerRadVel >> 6;
		splash->InnerRadVel -= splash->InnerRadVel >> 6;
		splash->MiddleRad += splash->MiddleRadVel >> 5;
		splash->MiddleSize += splash->MiddleRadVel >> 6;
		splash->MiddleRadVel -= splash->MiddleRadVel >> 6;
		splash->OuterRad += splash->OuterRadVel >> 5;
		splash->OuterSize += splash->OuterRadVel >> 6;
		splash->OuterRadVel -= splash->OuterRadVel >> 6;
		splash->InnerY += splash->InnerYVel >> 4;
		splash->InnerYVel += 0x400;

		if(splash->InnerYVel > 0x4000) {
			splash->InnerYVel = 0x4000;
		}

		if(splash->InnerY < 0) {
			if(splash->InnerY < -0x7000) {
				splash->InnerY = -0x7000;
			}
		} else {
			splash->InnerY = 0;
			splash->flags |= 4;
			splash->life -= 1;

			if(!splash->life) {
				splash->flags = 0;
			}
		}

		splash->MiddleY += splash->MiddleYVel >> 4;
		splash->MiddleYVel += 0x380;

		if(splash->MiddleYVel > 0x4000) {
			splash->MiddleYVel = 0x4000;
		}

		if(splash->MiddleY < 0) {
			if(splash->MiddleY < -0x7000) {
				splash->MiddleY = -0x7000;
			}
		} else {
			splash->MiddleY = 0;
			splash->flags |= 8;
		}
	}

	for(int i = 0; i < nRipples; i++) {
		ripple = &ripples[i];

		if(!(ripple->flags & 1)) {
			continue;
		}

		if(ripple->size < 252) {
			if(ripple->flags & 2) {
				ripple->size += 3;
			} else {
				ripple->size += 7;
			}
		}

		if(ripple->init) {
			if(ripple->init < ripple->life) {
				if(ripple->flags & 2) {
					ripple->init += 8;
				} else {
					ripple->init += 4;
				}

				if(ripple->init >= ripple->life) {
					ripple->init = 0;
				}
			}
		} else {
			ripple->life -= 3;

			if(ripple->life > 250) {
				ripple->flags = 0;
			}
		}
	}
}
