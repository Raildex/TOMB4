
#include "game/moveblok.h"
#include "game/animstruct.h"
#include "game/box.h"
#include "game/boxinfo.h"
#include "game/collide.h"
#include "game/control.h"
#include "game/delstuff.h"
#include "game/deltapak.h"
#include "game/draw.h"
#include "game/floorinfo.h"
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
#include "game/levelinfo.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/quadrantnames.h"
#include "game/roominfo.h"
#include "game/sound.h"
#include "game/sphere.h"
#include "game/tomb4fx.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/input.h"
#include "specific/output.h"
#include <stdlib.h>
#include <windows.h>


static short MovingBlockBounds[12] = { 0, 0, -256, 0, 0, 0, -1820, 1820, -5460, 5460, -1820, 1820 };

static PHD_VECTOR MovingBlockPos = { 0, 0, 0 };

static void ClearMovableBlockSplitters(long x, long y, long z, short room_number) {
	FLOOR_INFO* floor;
	short room_num, height;

	floor = GetFloor(x, y, z, &room_number);
	GetBox(currentLevel, floor->box)->overlap_index = ~0x4000;
	height = GetBox(currentLevel, floor->box)->height;
	room_num = room_number;
	floor = GetFloor(x + 1024, y, z, &room_number);

	if(floor->box != 0x7FF) {
		if(GetBox(currentLevel, floor->box)->height == height && GetBox(currentLevel, floor->box)->overlap_index & 0x8000
		   && GetBox(currentLevel, floor->box)->overlap_index & 0x4000) {
			ClearMovableBlockSplitters(x + 1024, y, z, room_number);
		}
	}

	room_number = room_num;
	floor = GetFloor(x - 1024, y, z, &room_number);

	if(floor->box != 0x7FF) {
		if(GetBox(currentLevel, floor->box)->height == height && GetBox(currentLevel, floor->box)->overlap_index & 0x8000
		   && GetBox(currentLevel, floor->box)->overlap_index & 0x4000) {
			ClearMovableBlockSplitters(x - 1024, y, z, room_number);
		}
	}

	room_number = room_num;
	floor = GetFloor(x, y, z + 1024, &room_number);

	if(floor->box != 0x7FF) {
		if(GetBox(currentLevel, floor->box)->height == height && GetBox(currentLevel, floor->box)->overlap_index & 0x8000
		   && GetBox(currentLevel, floor->box)->overlap_index & 0x4000) {
			ClearMovableBlockSplitters(x, y, z + 1024, room_number);
		}
	}

	room_number = room_num;
	floor = GetFloor(x, y, z - 1024, &room_number);

	if(floor->box != 0x7FF) {
		if(GetBox(currentLevel, floor->box)->height == height && GetBox(currentLevel, floor->box)->overlap_index & 0x8000
		   && GetBox(currentLevel, floor->box)->overlap_index & 0x4000) {
			ClearMovableBlockSplitters(x, y, z - 1024, room_number);
		}
	}
}

void InitialiseMovingBlock(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);
	ClearMovableBlockSplitters(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, item->room_number);
}

static long TestBlockPush(ITEM_INFO* item, long height, unsigned short quadrant) {
	ITEM_INFO* itemlist[6] = { 0 };
	ITEM_INFO* collided;
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	ROOM_INFO* r;
	long x, y, z, rx, rz;
	short room_number;

	x = item->pos.pos.x;
	y = item->pos.pos.y;
	z = item->pos.pos.z;

	switch(quadrant) {
	case NORTH:
		z += 1024;
		break;

	case EAST:
		x += 1024;
		break;

	case SOUTH:
		z -= 1024;
		break;

	case WEST:
		x -= 1024;
		break;
	}

	room_number = item->room_number;
	floor = GetFloor(x, y - 256, z, &room_number);
	r = GetRoom(currentLevel, room_number);
	rx = (x - r->x) >> 10;
	rz = (z - r->z) >> 10;

	if(r->floor[rx * r->x_size + rz].stopper) {
		return 0;
	}

	if(GetHeight(floor, x, y - 256, z, &height_type, &tiltxoff, &tiltzoff, &OnObject) != y) {
		return 0;
	}

	GetHeight(floor, x, y, z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

	if(height_type != WALL) {
		return 0;
	}

	y -= height - 100;
	floor = GetFloor(x, y, z, &room_number);

	if(GetCeiling(floor, x, y, z) > y) {
		return 0;
	}

	rx = item->pos.pos.x;
	rz = item->pos.pos.z;
	item->pos.pos.x = x;
	item->pos.pos.z = z;
	GetCollidedObjects(item, 256, 1, itemlist, 5, NULL, 0, 0);
	item->pos.pos.x = rx;
	item->pos.pos.z = rz;

	if(itemlist[0]) {
		for(int i = 0; itemlist[i] != 0; i++) {
			collided = itemlist[i];

			if(collided->object_number == TWOBLOCK_PLATFORM || collided->object_number == HAMMER) {
				return 1;
			}
		}

		return 0;
	}

	return 1;
}

static long TestBlockPull(ITEM_INFO* item, long height, unsigned short quadrant) {
	ITEM_INFO* collided;
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	ROOM_INFO* r;
	long x, y, z, destx, destz, rx, rz, ignore;
	short room_number;
	ITEM_INFO* itemlist[6] = { 0 };
	destx = 0;
	destz = 0;

	switch(quadrant) {
	case NORTH:
		destz = -1024;
		break;

	case EAST:
		destx = -1024;
		break;

	case SOUTH:
		destz = 1024;
		break;

	case WEST:
		destx = 1024;
		break;
	}

	x = item->pos.pos.x + destx;
	y = item->pos.pos.y;
	z = item->pos.pos.z + destz;
	room_number = item->room_number;
	floor = GetFloor(x, y - 256, z, &room_number);
	r = GetRoom(currentLevel, room_number);
	rx = (x - r->x) >> 10;
	rz = (z - r->z) >> 10;

	if(r->floor[rx * r->x_size + rz].stopper) {
		return 0;
	}

	if(GetHeight(floor, x, y - 256, z, &height_type, &tiltxoff, &tiltzoff, &OnObject) != y) {
		return 0;
	}

	floor = GetFloor(x, y - height, z, &room_number);

	if(floor->ceiling << 8 > y - height) {
		return 0;
	}

	rx = item->pos.pos.x;
	rz = item->pos.pos.z;
	item->pos.pos.x = x;
	item->pos.pos.z = z;
	GetCollidedObjects(item, 256, 1, itemlist, 4, NULL, 0, 0);
	item->pos.pos.x = rx;
	item->pos.pos.z = rz;

	if(itemlist[0]) {
		ignore = 0;

		for(int i = 0; itemlist[i] != 0; i++) {
			collided = itemlist[i];

			if(collided->object_number == TWOBLOCK_PLATFORM || collided->object_number == HAMMER) {
				ignore = 1;
				break;
			}
		}

		if(!ignore) {
			return 0;
		}
	}

	x += destx;
	z += destz;
	room_number = item->room_number;
	floor = GetFloor(x, y - 256, z, &room_number);

	if(GetHeight(floor, x, y - 256, z, &height_type, &tiltxoff, &tiltzoff, &OnObject) != y) {
		return 0;
	}

	floor = GetFloor(x, y - 762, z, &room_number);

	if(floor->ceiling << 8 > y - 762) {
		return 0;
	}

	x = lara_item->pos.pos.x + destx;
	y = lara_item->pos.pos.y;
	z = lara_item->pos.pos.z + destz;
	room_number = lara_item->room_number;
	GetFloor(x, y, z, &room_number);
	r = GetRoom(currentLevel, room_number);
	rx = (x - r->x) >> 10;
	rz = (z - r->z) >> 10;

	if(r->floor[rx * r->x_size + rz].stopper) {
		return 0;
	}

	rx = lara_item->pos.pos.x;
	rz = lara_item->pos.pos.z;
	lara_item->pos.pos.x = x;
	lara_item->pos.pos.z = z;
	GetCollidedObjects(lara_item, 256, 1, itemlist, 5, NULL, 0, 0);
	lara_item->pos.pos.x = rx;
	lara_item->pos.pos.z = rz;

	if(itemlist[0]) {
		for(int i = 0; itemlist[i] != 0; i++) {
			collided = itemlist[i];

			if(collided == item || collided->object_number == TWOBLOCK_PLATFORM || collided->object_number == HAMMER) {
				return 1;
			}
		}

		return 0;
	}

	return 1;
}

void MovableBlock(short item_number) {
	ITEM_INFO* item;
	PHD_VECTOR pos;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	long offset;
	unsigned short quadrant;
	short frame, base, room_number;
	static char sfx = 0;

	item = GetItem(currentLevel, item_number);
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	quadrant = (unsigned short)(lara_item->pos.y_rot + 0x2000) / 0x4000;

	switch(lara_item->anim_number) {
	case ANIM_PUSH:
		frame = lara_item->frame_number;
		base = GetAnim(currentLevel, ANIM_PUSH)->frame_base;

		if((frame < base + 30 || frame > base + 67) && (frame < base + 78 || frame > base + 125)
		   && (frame < base + 140 || frame > base + 160)) {
			if(sfx) {
				SoundEffect(SFX_PUSH_BLOCK_END, &item->pos, SFX_ALWAYS);
				sfx = 0;
			}
		} else {
			SoundEffect(SFX_PUSHABLE_SOUND, &item->pos, SFX_ALWAYS);
			sfx = 1;
		}

		GetLaraJointPos(&pos, 14);

		switch(quadrant) {
		case NORTH:
			offset = pos.z + *(long*)&item->item_flags[2] - *(long*)&lara_item->item_flags[2];

			if(abs(item->pos.pos.z - offset) < 512 && item->pos.pos.z < offset) {
				item->pos.pos.z = offset;
			}

			break;

		case EAST:
			offset = pos.x + *(long*)item->item_flags - *(long*)lara_item->item_flags;

			if(abs(item->pos.pos.x - offset) < 512 && item->pos.pos.x < offset) {
				item->pos.pos.x = offset;
			}

			break;

		case SOUTH:
			offset = pos.z + *(long*)&item->item_flags[2] - *(long*)&lara_item->item_flags[2];

			if(abs(item->pos.pos.z - offset) < 512 && item->pos.pos.z > offset) {
				item->pos.pos.z = offset;
			}

			break;

		case WEST:
			offset = pos.x + *(long*)item->item_flags - *(long*)lara_item->item_flags;

			if(abs(item->pos.pos.x - offset) < 512 && item->pos.pos.x > offset) {
				item->pos.pos.x = offset;
			}

			break;
		}


		if(lara_item->frame_number == GetAnim(currentLevel, lara_item->anim_number)->frame_end - 1) {
			if(S_IsActionDown(inputImpl, IN_ACTION)) {
				if(!TestBlockPush(item, 1024, quadrant)) {
					lara_item->goal_anim_state = 2;
				}
			} else {
				lara_item->goal_anim_state = 2;
			}
		}

		break;

	case ANIM_PULL:
		frame = lara_item->frame_number;
		base = GetAnim(currentLevel, ANIM_PULL)->frame_base;

		if((frame < base + 40 || frame > base + 122) && (frame < base + 130 || frame > base + 170)) {
			if(sfx) {
				SoundEffect(SFX_PUSH_BLOCK_END, &item->pos, SFX_ALWAYS);
				sfx = 0;
			}
		} else {
			SoundEffect(SFX_PUSHABLE_SOUND, &item->pos, SFX_ALWAYS);
			sfx = 1;
		}

		GetLaraJointPos(&pos, 14);

		switch(quadrant) {
		case NORTH:
			offset = pos.z + *(long*)&item->item_flags[2] - *(long*)&lara_item->item_flags[2];

			if(abs(item->pos.pos.z - offset) < 512 && item->pos.pos.z > offset) {
				item->pos.pos.z = offset;
			}

			break;

		case EAST:
			offset = pos.x + *(long*)item->item_flags - *(long*)lara_item->item_flags;

			if(abs(item->pos.pos.x - offset) < 512 && item->pos.pos.x > offset) {
				item->pos.pos.x = offset;
			}

			break;

		case SOUTH:
			offset = pos.z + *(long*)&item->item_flags[2] - *(long*)&lara_item->item_flags[2];

			if(abs(item->pos.pos.z - offset) < 512 && item->pos.pos.z < offset) {
				item->pos.pos.z = offset;
			}

			break;

		case WEST:
			offset = pos.x + *(long*)item->item_flags - *(long*)lara_item->item_flags;

			if(abs(item->pos.pos.x - offset) < 512 && item->pos.pos.x < offset) {
				item->pos.pos.x = offset;
			}

			break;
		}

		if(lara_item->frame_number == GetAnim(currentLevel, lara_item->anim_number)->frame_end - 1) {
			if(S_IsActionDown(inputImpl, IN_ACTION)) {
				if(!TestBlockPull(item, 1024, quadrant)) {
					lara_item->goal_anim_state = 2;
				}
			} else {
				lara_item->goal_anim_state = 2;
			}
		}

		break;

	case 417:
	case 418:
		frame = lara_item->frame_number;

		if(frame == GetAnim(currentLevel, 417)->frame_base || frame == GetAnim(currentLevel, 418)->frame_base) {
			item->pos.pos.x = (item->pos.pos.x & -512) | 512;
			item->pos.pos.z = (item->pos.pos.z & -512) | 512;
		}

		if(frame == GetAnim(currentLevel, lara_item->anim_number)->frame_end) {
			room_number = item->room_number;
			GetHeight(
				GetFloor(item->pos.pos.x, item->pos.pos.y - 256, item->pos.pos.z, &room_number), item->pos.pos.x,
				item->pos.pos.y - 256, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
			TestTriggers(trigger_index, 1, item->flags & IFL_CODEBITS);
			RemoveActiveItem(item_number);
			item->status = ITEM_INACTIVE;
		}

		break;
	}
}

void MovableBlockCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll) {
	ITEM_INFO* item;
	PHD_VECTOR pos;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	short bounds[6];
	short room_number, yrot, quadrant;

	item = GetItem(currentLevel, item_number);
	room_number = item->room_number;
	item->pos.pos.y = GetHeight(
		GetFloor(item->pos.pos.x, item->pos.pos.y - 256, item->pos.pos.z, &room_number), item->pos.pos.x,
		item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

	if(item->room_number != room_number) {
		ItemNewRoom(item_number, room_number);
	}

	if(S_IsActionDown(inputImpl, IN_ACTION) && laraitem->current_anim_state == AS_STOP && laraitem->anim_number == ANIM_BREATH
		&& !laraitem->gravity_status && lara.gun_status == LG_NO_ARMS && item->status == ITEM_INACTIVE
		&& item->trigger_flags >= 0
		|| (lara.IsMoving && lara.GeneralPtr == item_number)) {
		room_number = laraitem->room_number;
		GetFloor(item->pos.pos.x, item->pos.pos.y - 256, item->pos.pos.z, &room_number);

		if(room_number == item->room_number) {
			GetBoundsAccurate(item, bounds);
			MovingBlockBounds[0] = bounds[0] - 100;
			MovingBlockBounds[1] = bounds[1] + 100;
			MovingBlockBounds[4] = bounds[4] - 200;
			MovingBlockBounds[5] = 0;
			yrot = item->pos.y_rot;
			item->pos.y_rot = (laraitem->pos.y_rot + 0x2000) & 0xC000;

			if(TestLaraPosition(MovingBlockBounds, item, laraitem)) {
				if(((unsigned short)(yrot + 0x2000) / 0x4000) + ((unsigned short)item->pos.y_rot / 0x4000) & 1) {
					MovingBlockPos.z = bounds[0] - 35;
				} else {
					MovingBlockPos.z = bounds[4] - 35;
				}

				if(MoveLaraPosition(&MovingBlockPos, item, laraitem)) {
					laraitem->anim_number = ANIM_PPREADY;
					laraitem->frame_number = GetAnim(currentLevel, ANIM_PPREADY)->frame_base;
					laraitem->current_anim_state = AS_PPREADY;
					laraitem->goal_anim_state = AS_PPREADY;
					lara.IsMoving = 0;
					lara.gun_status = LG_HANDS_BUSY;
					lara.CornerX = (long)item;
				} else {
					lara.GeneralPtr = item_number;
				}
			} else if(lara.IsMoving && lara.GeneralPtr == item_number) {
				lara.IsMoving = 0;
				lara.gun_status = LG_NO_ARMS;
			}

			item->pos.y_rot = yrot;
		}
	} else if(
		laraitem->current_anim_state == AS_PPREADY
		&& laraitem->frame_number == GetAnim(currentLevel, ANIM_PPREADY)->frame_base + 19
		&& (ITEM_INFO*)lara.CornerX == item) {
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		quadrant = (unsigned short)(laraitem->pos.y_rot + 8192) >> W2V_SHIFT;

		if(S_IsActionDown(inputImpl, IN_FORWARD)) {
			if(!TestBlockPush(item, 1024, quadrant)) {
				return;
			}

			laraitem->goal_anim_state = AS_PUSHBLOCK;
		} else if(S_IsActionDown(inputImpl, IN_BACK)) {
			if(!TestBlockPull(item, 1024, quadrant)) {
				return;
			}

			laraitem->goal_anim_state = AS_PULLBLOCK;
		} else {
			return;
		}

		AddActiveItem(item_number);
		item->status = ITEM_ACTIVE;
		lara.head_x_rot = 0;
		lara.head_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.torso_y_rot = 0;
		GetLaraJointPos(&pos, 14);
		*(long*)&laraitem->item_flags[0] = pos.x;
		*(long*)&laraitem->item_flags[2] = pos.z;
		*(long*)&item->item_flags[0] = item->pos.pos.x;
		*(long*)&item->item_flags[2] = item->pos.pos.z;
	} else {
		ObjectCollision(item_number, laraitem, coll);
	}
}

void InitialisePlanetEffect(short item_number) {
	ITEM_INFO* item;
	ITEM_INFO* item2;
	char* pifl;
	unsigned char others[4];

	item = GetItem(currentLevel, item_number);
	item->mesh_bits = 0;

	for(int i = 0; i < GetNumLevelItems(currentLevel); i++) // get the pushable we are linked to
	{
		item2 = GetItem(currentLevel, i);

		if(item2->object_number >= PUSHABLE_OBJECT1 && item2->object_number <= PUSHABLE_OBJECT5
		   && item2->trigger_flags == item->trigger_flags) {
			item->item_flags[0] = i;
			break;
		}
	}

	if(item->trigger_flags == 1) // get other planet effects
	{
		for(int i = 0, j = 0; i < GetNumLevelItems(currentLevel); i++) {
			item2 = GetItem(currentLevel, i);

			if(item2->object_number == PLANET_EFFECT && item_number != i) {
				others[j++] = i;
			}
		}

		pifl = (char*)&item->item_flags[2];

		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++) {
				item2 = GetItem(currentLevel, others[j]);

				if(item2->trigger_flags == i + 2) {
					*pifl++ = others[j];
					break;
				}
			}
		}
	}
}

void ControlPlanetEffect(short item_number) {
	ITEM_INFO* item;
	ITEM_INFO* item2;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	char* pifl;
	long b, g;

	item = GetItem(currentLevel, item_number);

	if(!TriggerActive(item)) {
		return;
	}

	if(item->item_flags[0] > 0) {
		GetItem(currentLevel, item->item_flags[0])->trigger_flags
			= -GetItem(currentLevel, item->item_flags[0])->trigger_flags; // disable pushable :D
		item->item_flags[0] = NO_ITEM;
	}

	item->mesh_bits = 255;
	AnimateItem(item);

	if(item->trigger_flags == 1) {
		if((GetItem(currentLevel, LOBYTE(item->item_flags[2]))->flags & IFL_CODEBITS) == IFL_CODEBITS
		   && (GetItem(currentLevel, HIBYTE(item->item_flags[2]))->flags & IFL_CODEBITS) == IFL_CODEBITS
		   && (GetItem(currentLevel, LOBYTE(item->item_flags[3]))->flags & IFL_CODEBITS) == IFL_CODEBITS
		   && (GetItem(currentLevel, HIBYTE(item->item_flags[3]))->flags & IFL_CODEBITS) == IFL_CODEBITS) {
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetJointAbsPosition(item, &pos, 0);

			item2 = find_a_fucking_item(ANIMATING4);
			pos2.x = 0;
			pos2.y = 0;
			pos2.z = 0;
			GetJointAbsPosition(item2, &pos2, 0);

			b = (GetRandomControl() & 0x1F) + 224;
			g = b - (GetRandomControl() & 0x3F);
			TriggerLightningGlow(pos.x, pos.y, pos.z, RGBA(0, g, b, (GetRandomControl() & 0x1F) + 48));
			TriggerLightningGlow(pos2.x, pos2.y, pos2.z, RGBA(0, g, b, (GetRandomControl() & 0x1F) + 64));

			if(!(GlobalCounter & 3)) {
				TriggerLightning(&pos, &pos2, (GetRandomControl() & 0x1F) + 32, RGBA(0, g, b, 24), 1, 32, 5);
			}

			pifl = (char*)&item->item_flags[2];

			for(int i = 0; i < 4; i++) {
				pos2.x = 0;
				pos2.y = 0;
				pos2.z = 0;
				GetJointAbsPosition(GetItem(currentLevel, pifl[i]), &pos2, 0);

				if(!(GlobalCounter & 3)) {
					TriggerLightning(&pos2, &pos, (GetRandomControl() & 0x1F) + 32, RGBA(0, g, b, 24), 1, 32, 5);
				}

				TriggerLightningGlow(pos.x, pos.y, pos.z, RGBA(0, g, b, (GetRandomControl() & 0x1F) + 48));
				SoundEffect(SFX_ELEC_ARCING_LOOP, (PHD_3DPOS*)&pos, SFX_DEFAULT);
				pos = pos2;
			}

			TriggerLightningGlow(pos2.x, pos2.y, pos2.z, RGBA(0, g, b, (GetRandomControl() & 0x1F) + 48));
		}
	}
}

void DrawPlanetEffect(ITEM_INFO* item) {
	OBJECT_INFO* obj;
	short** meshpp;
	long* bone;
	short* frm[2];
	short* rot;
	long poppush;

	if(!item->mesh_bits) {
		return;
	}

	GetFrames(item, frm, &poppush);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	CalculateObjectLighting(item, frm[0]);

	obj = GetObjectInfo(currentLevel, item->object_number);
	meshpp = GetMeshPointer(currentLevel, obj->mesh_index);
	bone = GetBone(currentLevel, obj->bone_index);
	phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
	rot = frm[0] + 9;
	gar_RotYXZsuperpack(&rot, 0);
	phd_PutPolygons(*meshpp, -1);
	phd_PutPolygons(*meshpp, -1);
	meshpp += 2;

	for(int i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp += 2) {
		poppush = bone[0];

		// These look inverted..
		if(poppush & 1) {
			phd_PopMatrix();
		}

		if(poppush & 2) {
			phd_PushMatrix();
		}

		phd_TranslateRel(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack(&rot, 0);
		phd_PutPolygons(*meshpp, -1);
	}

	phd_PopMatrix();
}
