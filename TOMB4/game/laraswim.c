
#include "game/laraswim.h"
#include "game/animstruct.h"
#include "game/camera.h"
#include "game/camerainfo.h"
#include "game/collide.h"
#include "game/collisiontypes.h"
#include "game/control.h"
#include "game/effect2.h"
#include "game/floorinfo.h"
#include "game/heighttypes.h"
#include "game/inputbuttons.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/lara.h"
#include "game/lara_states.h"
#include "game/larafire.h"
#include "game/laragunstatus.h"
#include "game/larainfo.h"
#include "game/laramesh.h"
#include "game/laramisc.h"
#include "game/larawaterstatus.h"
#include "game/levelinfo.h"
#include "game/objectvector.h"
#include "game/roomflags.h"
#include "game/roominfo.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/input.h"
#include <stdlib.h>

static void lara_as_swimcheat(ITEM_INFO* item, COLL_INFO* coll) {
	if(S_IsActionDown(inputImpl, IN_FORWARD)) {
		item->pos.x_rot -= 546;
	} else if(S_IsActionDown(inputImpl, IN_BACK)) {
		item->pos.x_rot += 546;
	}

	if(S_IsActionDown(inputImpl, IN_LEFT)) {
		lara.turn_rate -= 613;

		if(lara.turn_rate < -1092) {
			lara.turn_rate = -1092;
		}
	} else if(S_IsActionDown(inputImpl, IN_RIGHT)) {
		lara.turn_rate += 613;

		if(lara.turn_rate > 1092) {
			lara.turn_rate = 1092;
		}
	}

	if(S_IsActionDown(inputImpl, IN_ACTION)) {
		TriggerDynamic(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, 31, 255, 255, 255);
	}

	if(S_IsActionDown(inputImpl, IN_ROLL)) {
		lara.turn_rate = -2184;
	}

	if(S_IsActionDown(inputImpl, IN_JUMP)) {
		item->fallspeed += 16;

		if(item->fallspeed > 400) {
			item->fallspeed = 400;
		}
	} else {
		if(item->fallspeed >= 8) {
			item->fallspeed -= item->fallspeed >> 3;
		} else {
			item->fallspeed = 0;
		}
	}
}

void lara_as_swim(ITEM_INFO* item, COLL_INFO* coll) {
	if(item->hit_points <= 0) {
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	if(S_IsActionDown(inputImpl, IN_ROLL)) {
		item->current_anim_state = AS_WATERROLL;
		item->anim_number = ANIM_WATERROLL;
		item->frame_number = GetAnim(currentLevel, ANIM_WATERROLL)->frame_base;
	} else {
		SwimTurn(item);
		item->fallspeed += 8;

		if(item->fallspeed > 200) {
			item->fallspeed = 200;
		}

		if(!(S_IsActionDown(inputImpl, IN_JUMP))) {
			item->goal_anim_state = AS_GLIDE;
		}
	}
}

void LaraUnderWater(ITEM_INFO* item, COLL_INFO* coll) {
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -400;
	coll->bad_ceiling = 400;
	coll->old.x = item->pos.pos.x;
	coll->old.y = item->pos.pos.y;
	coll->old.z = item->pos.pos.z;
	coll->radius = lara.water_status == LW_FLYCHEAT ? 100 : 300;
	coll->trigger = 0;
	coll->slopes_are_walls = 0;
	coll->slopes_are_pits = 0;
	coll->lava_is_pit = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;

	if(S_IsActionDown(inputImpl, IN_LOOK) && lara.look) {
		LookLeftRight();
	} else {
		ResetLook();
	}

	lara.look = 1;
	lara_control_routines[item->current_anim_state](item, coll);

	if(item->pos.z_rot < -364) {
		item->pos.z_rot += 364;
	} else if(item->pos.z_rot > 364) {
		item->pos.z_rot -= 364;
	} else {
		item->pos.z_rot = 0;
	}

	if(item->pos.x_rot < -15470) {
		item->pos.x_rot = -15470;
	} else if(item->pos.x_rot > 15470) {
		item->pos.x_rot = 15470;
	}

	if(item->pos.z_rot < -4004) {
		item->pos.z_rot = -4004;
	} else if(item->pos.z_rot > 4004) {
		item->pos.z_rot = 4004;
	}

	if(lara.turn_rate < -364) {
		lara.turn_rate += 364;
	} else if(lara.turn_rate > 364) {
		lara.turn_rate -= 364;
	} else {
		lara.turn_rate = 0;
	}

	item->pos.y_rot += lara.turn_rate;

	if(lara.current_active && lara.water_status != LW_FLYCHEAT) {
		LaraWaterCurrent(coll);
	}

	AnimateLara(item);
	item->pos.pos.x += (((phd_sin(item->pos.y_rot) * item->fallspeed) >> 16) * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
	item->pos.pos.y -= (phd_sin(item->pos.x_rot) * item->fallspeed) >> 16;
	item->pos.pos.z += (((phd_cos(item->pos.y_rot) * item->fallspeed) >> 16) * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
	LaraBaddieCollision(item, coll);

	if(lara.vehicle == NO_ITEM) {
		lara_collision_routines[item->current_anim_state](item, coll);
	}

	UpdateLaraRoom(item, 0);
	LaraGun();
	TestTriggers(coll->trigger, 0, 0);

	if(lara.water_status == LW_FLYCHEAT) {
		item->anim_number = ANIM_FASTFALL;
		item->frame_number = GetAnim(currentLevel, ANIM_FASTFALL)->frame_base + 5;
	}
}

void lara_col_swim(ITEM_INFO* item, COLL_INFO* coll) {
	LaraSwimCollision(item, coll);
}

void lara_col_glide(ITEM_INFO* item, COLL_INFO* coll) {
	LaraSwimCollision(item, coll);
}

void lara_col_tread(ITEM_INFO* item, COLL_INFO* coll) {
	LaraSwimCollision(item, coll);
}

void lara_col_dive(ITEM_INFO* item, COLL_INFO* coll) {
	LaraSwimCollision(item, coll);
}

void lara_col_waterroll(ITEM_INFO* item, COLL_INFO* coll) {
	LaraSwimCollision(item, coll);
}

void lara_as_glide(ITEM_INFO* item, COLL_INFO* coll) {
	if(lara.water_status == LW_FLYCHEAT) {
		lara_as_swimcheat(item, coll);
	} else if(lara.water_status != LW_ABOVE_WATER) {
		if(item->hit_points <= 0) {
			item->goal_anim_state = AS_UWDEATH;
			return;
		}

		if(S_IsActionDown(inputImpl, IN_ROLL)) {
			item->current_anim_state = AS_WATERROLL;
			item->anim_number = ANIM_WATERROLL;
			item->frame_number = GetAnim(currentLevel, ANIM_WATERROLL)->frame_base;
		} else {
			SwimTurn(item);

			if(S_IsActionDown(inputImpl, IN_JUMP)) {
				item->goal_anim_state = AS_SWIM;
			}

			item->fallspeed -= 6;

			if(item->fallspeed < 0) {
				item->fallspeed = 0;
			}

			if(item->fallspeed <= 133) {
				item->goal_anim_state = AS_TREAD;
			}
		}
	}
}

void lara_as_tread(ITEM_INFO* item, COLL_INFO* coll) {
	if(item->hit_points <= 0) {
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	if(S_IsActionDown(inputImpl, IN_ROLL)) {
		item->current_anim_state = AS_WATERROLL;
		item->anim_number = ANIM_WATERROLL;
		item->frame_number = GetAnim(currentLevel, ANIM_WATERROLL)->frame_base;
		return;
	}

	if(S_IsActionDown(inputImpl, IN_LOOK)) {
		LookUpDown();
	}

	SwimTurn(item);

	if(S_IsActionDown(inputImpl, IN_JUMP)) {
		item->goal_anim_state = AS_SWIM;
	}

	item->fallspeed -= 6;

	if(item->fallspeed < 0) {
		item->fallspeed = 0;
	}

	if(lara.gun_status == LG_HANDS_BUSY) {
		lara.gun_status = LG_NO_ARMS;
	}
}

void lara_as_dive(ITEM_INFO* item, COLL_INFO* coll) {
	if(S_IsActionDown(inputImpl, IN_FORWARD)) {
		item->pos.x_rot -= 182;
	}
}

void lara_as_uwdeath(ITEM_INFO* item, COLL_INFO* coll) {
	lara.look = 0;
	item->fallspeed -= 8;

	if(item->fallspeed <= 0) {
		item->fallspeed = 0;
	}

	if(item->pos.x_rot >= -364 && item->pos.x_rot <= 364) {
		item->pos.x_rot = 0;
	} else if(item->pos.x_rot < 0) {
		item->pos.x_rot += 364;
	} else {
		item->pos.x_rot -= 364;
	}
}

void lara_as_waterroll(ITEM_INFO* item, COLL_INFO* coll) {
	camera.node = LM_HEAD;
	if(lara.water_status == LW_FLYCHEAT) {
		item->current_anim_state = AS_GLIDE;
	} else {
		item->fallspeed = 0;
	}
}

void lara_col_uwdeath(ITEM_INFO* item, COLL_INFO* coll) {
	long wh;

	item->hit_points = -1;
	lara.air = -1;
	lara.gun_status = LG_HANDS_BUSY;
	wh = GetWaterHeight(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, item->room_number);

	if(wh != NO_HEIGHT && wh < item->pos.pos.y - 100) {
		item->pos.pos.y -= 5;
	}

	LaraSwimCollision(item, coll);
}

long GetWaterDepth(long x, long y, long z, short room_number) {
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	height_types ht;
	long tiltxoff, tiltzoff, OnObject;
	long x_floor, y_floor, h;
	short door;

	r = GetRoom(currentLevel, room_number);

	do {
		x_floor = (z - r->z) >> 10;
		y_floor = (x - r->x) >> 10;

		if(x_floor <= 0) {
			x_floor = 0;

			if(y_floor < 1) {
				y_floor = 1;
			} else if(y_floor > r->y_size - 2) {
				y_floor = r->y_size - 2;
			}
		} else if(x_floor >= r->x_size - 1) {
			x_floor = r->x_size - 1;

			if(y_floor < 1) {
				y_floor = 1;
			} else if(y_floor > r->y_size - 2) {
				y_floor = r->y_size - 2;
			}
		} else if(y_floor < 0) {
			y_floor = 0;
		} else if(y_floor >= r->y_size) {
			y_floor = r->y_size - 1;
		}

		floor = &r->floor[x_floor + y_floor * r->x_size];
		door = GetDoor(floor);

		if(door != 255) {
			room_number = door;
			r = GetRoom(currentLevel, door);
		}

	} while(door != 255);

	if(r->flags & ROOM_UNDERWATER) {
		while(floor->sky_room != 255) {
			r = GetRoom(currentLevel, floor->sky_room);

			if(!(r->flags & ROOM_UNDERWATER)) {
				h = GetMinimumCeiling(floor, x, z);
				floor = GetFloor(x, y, z, &room_number);
				return GetHeight(floor, x, y, z, &ht, &tiltxoff, &tiltzoff, &OnObject) - h;
			}

			floor = &r->floor[((z - r->z) >> 10) + r->x_size * ((x - r->x) >> 10)];
		}

		return 0x7FFF;
	} else {
		while(floor->pit_room != 255) {
			r = GetRoom(currentLevel, floor->pit_room);

			if(r->flags & ROOM_UNDERWATER) {
				h = GetMaximumFloor(floor, x, z);
				floor = GetFloor(x, y, z, &room_number);
				return GetHeight(floor, x, y, z, &ht, &tiltxoff, &tiltzoff, &OnObject) - h;
			}

			floor = &r->floor[((z - r->z) >> 10) + r->x_size * ((x - r->x) >> 10)];
		}

		return NO_HEIGHT;
	}
}

void SwimTurn(ITEM_INFO* item) {
	if(S_IsActionDown(inputImpl, IN_FORWARD)) {
		item->pos.x_rot -= 364;
	} else if(S_IsActionDown(inputImpl, IN_BACK)) {
		item->pos.x_rot += 364;
	}

	if(S_IsActionDown(inputImpl, IN_LEFT)) {
		lara.turn_rate -= 409;

		if(lara.turn_rate < -1092) {
			lara.turn_rate = -1092;
		}

		item->pos.z_rot -= 546;
	} else if(S_IsActionDown(inputImpl, IN_RIGHT)) {
		lara.turn_rate += 409;

		if(lara.turn_rate > 1092) {
			lara.turn_rate = 1092;
		}

		item->pos.z_rot += 546;
	}
}

void LaraTestWaterDepth(ITEM_INFO* item, COLL_INFO* coll) {
	FLOOR_INFO* floor;
	height_types ht;
	long tiltxoff, tiltzoff, OnObject;
	long wd;
	short room_number;

	room_number = item->room_number;
	floor = GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);
	wd = GetWaterDepth(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, room_number);

	if(wd == NO_HEIGHT) {
		item->pos.pos.x = coll->old.x;
		item->pos.pos.y = coll->old.y;
		item->pos.pos.z = coll->old.z;
		item->fallspeed = 0;
	} else if(wd <= 512) {
		item->anim_number = ANIM_SWIM2QSTND;
		item->frame_number = GetAnim(currentLevel, ANIM_SWIM2QSTND)->frame_base;
		item->current_anim_state = AS_WATEROUT;
		item->goal_anim_state = AS_STOP;
		item->pos.x_rot = 0;
		item->pos.z_rot = 0;
		item->gravity_status = 0;
		item->speed = 0;
		item->fallspeed = 0;
		lara.water_status = LW_WADE;
		item->pos.pos.y = GetHeight(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &ht, &tiltxoff, &tiltzoff, &OnObject);
	}
}

void LaraSwimCollision(ITEM_INFO* item, COLL_INFO* coll) {
	COLL_INFO coll2, coll3;
	long height;
	short oxr, hit;

	hit = 0;
	oxr = item->pos.x_rot;

	if(oxr < -0x4000 || oxr > 0x4000) {
		lara.move_angle = item->pos.y_rot - 0x8000;
		coll->facing = item->pos.y_rot - 0x8000;
	} else {
		lara.move_angle = item->pos.y_rot;
		coll->facing = item->pos.y_rot;
	}

	height = 640 * phd_sin(item->pos.x_rot) >> W2V_SHIFT;

	if(height < 0) {
		height = -height;
	}


	coll->bad_neg = -64;
	coll2 = *coll;
	coll3 = *coll;

	GetCollisionInfo(coll, item->pos.pos.x, item->pos.pos.y + height / 2, item->pos.pos.z, item->room_number, height);

	coll2.facing += 0x2000;
	GetCollisionInfo(&coll2, item->pos.pos.x, item->pos.pos.y + height / 2, item->pos.pos.z, item->room_number, height);

	coll3.facing -= 0x2000;
	GetCollisionInfo(&coll3, item->pos.pos.x, item->pos.pos.y + height / 2, item->pos.pos.z, item->room_number, height);

	ShiftItem(item, coll);

	switch(coll->coll_type) {
	case CT_FRONT:

		if(item->pos.x_rot > 4550) {
			item->pos.x_rot += 182;
			hit = 1;
		} else if(item->pos.x_rot < -4550) {
			item->pos.x_rot -= 182;
			hit = 1;
		} else if(item->pos.x_rot > 910) {
			item->pos.x_rot += 91;
		} else if(item->pos.x_rot < -910) {
			item->pos.x_rot -= 91;
		} else if(item->pos.x_rot > 0) {
			item->pos.x_rot += 45;
		} else if(item->pos.x_rot < 0) {
			item->pos.x_rot -= 45;
		} else {
			hit = 1;
			item->fallspeed = 0;
		}

		if(coll2.coll_type == CT_LEFT) {
			item->pos.y_rot += 364;
		} else if(coll2.coll_type == CT_RIGHT) {
			item->pos.y_rot -= 364;
		} else if(coll3.coll_type == CT_LEFT) {
			item->pos.y_rot += 364;
		} else if(coll3.coll_type == CT_RIGHT) {
			item->pos.y_rot -= 364;
		}

		break;

	case CT_TOP:

		if(item->pos.x_rot >= -8190) {
			hit = 1;
			item->pos.x_rot -= 182;
		}

		break;

	case CT_TOP_FRONT:
		item->fallspeed = 0;
		hit = 1;
		break;

	case CT_LEFT:
		item->pos.y_rot += 364;
		hit = 1;
		break;

	case CT_RIGHT:
		item->pos.y_rot -= 364;
		hit = 1;
		break;

	case CT_CLAMP:
		hit = 2;
		item->pos.pos.x = coll->old.x;
		item->pos.pos.y = coll->old.y;
		item->pos.pos.z = coll->old.z;
		item->fallspeed = 0;
		break;
	}

	if(coll->mid_floor < 0 && coll->mid_floor != NO_HEIGHT) {
		hit = 1;
		item->pos.x_rot += 182;
		item->pos.pos.y += coll->mid_floor;
	}

	if(hit != 2 && lara.water_status != LW_FLYCHEAT) {
		LaraTestWaterDepth(item, coll);
	}
}

void LaraWaterCurrent(COLL_INFO* coll) {
	long angle, speed, sinkval, shifter, absvel;

	if(lara.current_active) {
		sinkval = lara.current_active - 1;
		speed = GetFixedCamera(currentLevel, sinkval)->data;
		angle = ((mGetAngle(GetFixedCamera(currentLevel, sinkval)->x, GetFixedCamera(currentLevel, sinkval)->z, lara_item->pos.pos.x, lara_item->pos.pos.z) - 0x4000) >> 4) & 0xFFF;
		lara.current_xvel += (short)((((speed * rcossin_tbl[2 * angle]) >> 2) - lara.current_xvel) >> 4);
		lara.current_zvel += (short)((((speed * rcossin_tbl[2 * angle + 1]) >> 2) - lara.current_zvel) >> 4);
		lara_item->pos.pos.y += (GetFixedCamera(currentLevel, sinkval)->y - lara_item->pos.pos.y) >> 4;
	} else {
		absvel = abs(lara.current_xvel);

		if(absvel > 16) {
			shifter = 4;
		} else if(absvel > 8) {
			shifter = 3;
		} else {
			shifter = 2;
		}

		lara.current_xvel -= lara.current_xvel >> shifter;

		if(abs(lara.current_xvel) < 4) {
			lara.current_xvel = 0;
		}

		absvel = abs(lara.current_zvel);

		if(absvel > 16) {
			shifter = 4;
		} else if(absvel > 8) {
			shifter = 3;
		} else {
			shifter = 2;
		}

		lara.current_zvel -= lara.current_zvel >> shifter;

		if(abs(lara.current_zvel) < 4) {
			lara.current_zvel = 0;
		}

		if(!lara.current_xvel && !lara.current_zvel) {
			return;
		}
	}

	lara_item->pos.pos.x += lara.current_xvel >> 8;
	lara_item->pos.pos.z += lara.current_zvel >> 8;
	lara.current_active = 0;
	coll->facing = (short)phd_atan(lara_item->pos.pos.z - coll->old.z, lara_item->pos.pos.x - coll->old.x);
	GetCollisionInfo(coll, lara_item->pos.pos.x, lara_item->pos.pos.y + 200, lara_item->pos.pos.z, lara_item->room_number, 400);

	switch(coll->coll_type) {
	case CT_FRONT:

		if(lara_item->pos.x_rot > 6370) {
			lara_item->pos.x_rot += 364;
		} else if(lara_item->pos.x_rot < -6370) {
			lara_item->pos.x_rot -= 364;
		} else {
			lara_item->fallspeed = 0;
		}

		break;

	case CT_TOP:
		lara_item->pos.x_rot -= 364;
		break;

	case CT_TOP_FRONT:
		lara_item->fallspeed = 0;
		break;

	case CT_LEFT:
		lara_item->pos.y_rot += 910;
		break;

	case CT_RIGHT:
		lara_item->pos.y_rot -= 910;
		break;
	}

	if(coll->mid_floor < 0 && coll->mid_floor != NO_HEIGHT) {
		lara_item->pos.pos.y += coll->mid_floor;
	}

	ShiftItem(lara_item, coll);
	coll->old.x = lara_item->pos.pos.x;
	coll->old.y = lara_item->pos.pos.y;
	coll->old.z = lara_item->pos.pos.z;
}
