
#include "game/camera.h"
#include "distances.h"
#include "game/camerainfo.h"
#include "game/cameratype.h"
#include "game/control.h"
#include "game/delstuff.h"
#include "game/deltapak.h"
#include "game/draw.h"
#include "game/effect2.h"
#include "game/inputbuttons.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/lara.h"
#include "game/larafire.h"
#include "game/larainfo.h"
#include "game/laramesh.h"
#include "game/levelinfo.h"
#include "game/objects.h"
#include "game/objectvector.h"
#include "game/oldcamera.h"
#include "game/phdvector.h"
#include "game/roomflags.h"
#include "game/roominfo.h"
#include "game/savegame.h"
#include "game/savegameinfo.h"
#include "game/shatteritem.h"
#include "game/sound.h"
#include "game/weapontypes.h"
#include "global/types.h"
#include "heighttypes.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/input.h"
#include "sphere.h"
#include <stdlib.h>

CAMERA_INFO camera;

GAME_VECTOR ForcedFixedCamera;
char UseForcedFixedCamera;

PHD_VECTOR LaraTorchStart;
PHD_VECTOR LaraTorchEnd;
long bLaraTorch;
long LaraTorchIntensity;
long LaraTorchYRot;

camera_type BinocularOldCamera = CHASE_CAMERA;
long BinocularOn = 0;
long BinocularRange = 0;
long ExittingBinos = 0;
long LaserSight = 0;



static OLD_CAMERA old_cam;
static GAME_VECTOR last_target;
static GAME_VECTOR last_ideal;
static char TargetSnaps = 0;
static char camerasnaps = 0;

static GAME_VECTOR static_lookcamp;
static GAME_VECTOR static_lookcamt;

void InitialiseCamera() {
	last_target.pos.x = lara_item->pos.pos.x;
	camera.target.pos.x = last_target.pos.x;
	camera.shift = lara_item->pos.pos.y - 1024;
	last_target.pos.y = camera.shift;
	camera.target.pos.y = camera.shift;
	last_target.pos.z = lara_item->pos.pos.z;
	camera.target.pos.z = last_target.pos.z;
	camera.pos.pos.y = camera.shift;
	last_target.room_number = lara_item->room_number;
	camera.target.room_number = lara_item->room_number;
	camera.pos.pos.x = last_target.pos.x;
	camera.pos.pos.z = last_target.pos.z - 100;
	camera.pos.room_number = lara_item->room_number;
	camera.target_distance = 1536;
	camera.item = NULL;
	camera.number_frames = 1;
	camera.type = CHASE_CAMERA;
	camera.speed = 1;
	camera.flags = 0;
	camera.bounce = 0;
	camera.number = -1;
	camera.fixed_camera = 0;
	AlterFOV(14560);
	UseForcedFixedCamera = 0;
	CalculateCamera();
}

long mgLOS(GAME_VECTOR* start, GAME_VECTOR* target, long push) {
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	long x, y, z, h, c, cdiff, hdiff, dx, dy, dz, clipped, nc, i;
	short room_number, room_number2;

	dx = (target->pos.x - start->pos.x) >> 3;
	dy = (target->pos.y - start->pos.y) >> 3;
	dz = (target->pos.z - start->pos.z) >> 3;
	x = start->pos.x;
	y = start->pos.y;
	z = start->pos.z;
	room_number = start->room_number;
	room_number2 = room_number;
	nc = 0;
	clipped = 0;

	for(i = 0; i < 8; i++) {
		room_number = room_number2;
		floor = GetFloor(x, y, z, &room_number2);
		h = GetHeight(floor, x, y, z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
		c = GetCeiling(floor, x, y, z);

		if(h == NO_HEIGHT || c == NO_HEIGHT || c >= h) {
			if(!nc) {
				x += dx;
				y += dy;
				z += dz;
				continue;
			}

			clipped = 1;
			break;
		}

		if(y > h) {
			hdiff = y - h;

			if(hdiff < push) {
				y = h;
			} else {
				clipped = 1;
				break;
			}
		}

		if(y < c) {
			cdiff = c - y;

			if(cdiff < push) {
				y = c;
			} else {
				clipped = 1;
				break;
			}
		}

		nc = 1;
		x += dx;
		y += dy;
		z += dz;
	}

	if(i) {
		x -= dx;
		y -= dy;
		z -= dz;
	}

	target->pos.x = x;
	target->pos.y = y;
	target->pos.z = z;
	GetFloor(x, y, z, &room_number);
	target->room_number = room_number;
	return !clipped;
}


void MoveCamera(GAME_VECTOR* ideal, long speed) {
	FLOOR_INFO* floor;
	GAME_VECTOR temp1;
	GAME_VECTOR temp2;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	long height, ceiling, shake, rndval, wx, wy, wz, dx, dy, dz;
	short room_number;

	if(BinocularOn < 0) {
		speed = 1;
		BinocularOn++;
	}

	{
		old_cam.pos.x_rot = lara_item->pos.x_rot;
		old_cam.pos.y_rot = lara_item->pos.y_rot;
		old_cam.pos.z_rot = lara_item->pos.z_rot;
		old_cam.pos2.x_rot = lara.head_x_rot;
		old_cam.pos2.y_rot = lara.head_y_rot;
		old_cam.pos2.pos.x = lara.torso_x_rot;
		old_cam.pos2.pos.y = lara.torso_y_rot;
		old_cam.pos.pos.x = lara_item->pos.pos.x;
		old_cam.pos.pos.y = lara_item->pos.pos.y;
		old_cam.pos.pos.z = lara_item->pos.pos.z;
		old_cam.current_anim_state = lara_item->current_anim_state;
		old_cam.goal_anim_state = lara_item->goal_anim_state;
		old_cam.target_distance = camera.target_distance;
		old_cam.target_elevation = camera.target_elevation;
		old_cam.target_angle = camera.target_angle;
		last_ideal.pos.x = ideal->pos.x;
		last_ideal.pos.y = ideal->pos.y;
		last_ideal.pos.z = ideal->pos.z;
		last_ideal.room_number = ideal->room_number;
	}

	camera.pos.pos.x += (ideal->pos.x - camera.pos.pos.x) / speed;
	camera.pos.pos.y += (ideal->pos.y - camera.pos.pos.y) / speed;
	camera.pos.pos.z += (ideal->pos.z - camera.pos.pos.z) / speed;
	camera.pos.room_number = ideal->room_number;

	if(camera.bounce) {
		if(camera.bounce <= 0) {
			rndval = -camera.bounce;
			shake = rndval >> 1;
			camera.target.pos.x += GetRandomControl() % rndval - shake;
			camera.target.pos.y += GetRandomControl() % rndval - shake;
			camera.target.pos.z += GetRandomControl() % rndval - shake;
			camera.bounce += 5;
		} else {
			camera.pos.pos.y += camera.bounce;
			camera.target.pos.y += camera.bounce;
			camera.bounce = 0;
		}
	}

	wz = camera.pos.pos.z;
	wy = camera.pos.pos.y;
	wx = camera.pos.pos.x;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	height = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	ceiling = GetCeiling(floor, wx, wy, wz);

	if(wy < ceiling || wy > height) {
		mgLOS(&camera.target, &camera.pos, 0);
		dx = abs(camera.pos.pos.x - ideal->pos.x);
		dy = abs(camera.pos.pos.y - ideal->pos.y);
		dz = abs(camera.pos.pos.z - ideal->pos.z);

		if(dx < 768 && dy < 768 && dz < 768) {
			temp1.pos.x = camera.pos.pos.x;
			temp1.pos.y = camera.pos.pos.y;
			temp1.pos.z = camera.pos.pos.z;
			temp1.room_number = camera.pos.room_number;
			temp2.pos.x = ideal->pos.x;
			temp2.pos.y = ideal->pos.y;
			temp2.pos.z = ideal->pos.z;
			temp2.room_number = ideal->room_number;

			if(!(mgLOS(&temp2, &temp1, 0))) {
				camerasnaps++;

				if(camerasnaps >= 8) {
					camera.pos.pos.x = ideal->pos.x;
					camera.pos.pos.y = ideal->pos.y;
					camera.pos.pos.z = ideal->pos.z;
					camera.pos.room_number = ideal->room_number;
					camerasnaps = 0;
				}
			}
		}
	}

	wz = camera.pos.pos.z;
	wy = camera.pos.pos.y;
	wx = camera.pos.pos.x;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	height = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	ceiling = GetCeiling(floor, wx, wy, wz);

	if(wy - 255 < ceiling && height < wy + 255 && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT) {
		camera.pos.pos.y = (height + ceiling) >> 1;
	} else if(height < wy + 255 && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT) {
		camera.pos.pos.y = height - 255;
	} else if(wy - 255 < ceiling && ceiling < height && ceiling != NO_HEIGHT && height != NO_HEIGHT) {
		camera.pos.pos.y = ceiling + 255;
	} else if(ceiling >= height || height == NO_HEIGHT || ceiling == NO_HEIGHT) {
		camera.pos.pos.x = ideal->pos.x;
		camera.pos.pos.y = ideal->pos.y;
		camera.pos.pos.z = ideal->pos.z;
		camera.pos.room_number = ideal->room_number;
	}

	GetFloor(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, camera.target.pos.x, camera.target.pos.y, camera.target.pos.z, 0);

	if(camera.mike_at_lara) {
		camera.actual_elevation = lara.head_y_rot + lara.torso_y_rot + lara_item->pos.y_rot;
		camera.mike_pos.x = lara_item->pos.pos.x;
		camera.mike_pos.y = lara_item->pos.pos.y;
		camera.mike_pos.z = lara_item->pos.pos.z;
	} else {
		dx = camera.target.pos.x - camera.pos.pos.x;
		dz = camera.target.pos.z - camera.pos.pos.z;
		dx = phd_atan(dz, dx);
		camera.actual_elevation = (short)dx;
		camera.mike_pos.x = camera.pos.pos.x + (phd_sin(dx) * phd_persp >> W2V_SHIFT);
		camera.mike_pos.y = camera.pos.pos.y;
		camera.mike_pos.z = camera.pos.pos.z + (phd_cos(dx) * phd_persp >> W2V_SHIFT);
	}

	camera.old_type = camera.type;
}


long CameraCollisionBounds(GAME_VECTOR* ideal, long push, long yfirst) {
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	long wx, wy, wz, h, c;
	short room_number;

	wx = ideal->pos.x;
	wy = ideal->pos.y;
	wz = ideal->pos.z;

	if(yfirst) {
		room_number = ideal->room_number;
		floor = GetFloor(wx, wy, wz, &room_number);
		h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
		c = GetCeiling(floor, wx, wy, wz);

		if(c > wy - 255 && h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT) {
			wy = (h + c) >> 1;
		} else if(h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT) {
			wy = h - 255;
		} else if(c > wy - 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT) {
			wy = c + 255;
		}
	}

	room_number = ideal->room_number;
	floor = GetFloor(wx - push, wy, wz, &room_number);
	h = GetHeight(floor, wx - push, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx - push, wy, wz);

	if(h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c) {
		wx = push + (wx & ~0x3FF);
	}

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz - push, &room_number);
	h = GetHeight(floor, wx, wy, wz - push, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx, wy, wz - push);

	if(h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c) {
		wz = push + (wz & ~0x3FF);
	}

	room_number = ideal->room_number;
	floor = GetFloor(wx + push, wy, wz, &room_number);
	h = GetHeight(floor, wx + push, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx + push, wy, wz);

	if(h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c) {
		wx = (wx | 0x3FF) - push;
	}

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz + push, &room_number);
	h = GetHeight(floor, wx, wy, wz + push, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx, wy, wz + push);

	if(h < wy || h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy < c) {
		wz = (wz | 0x3FF) - push;
	}

	if(!yfirst) {
		room_number = ideal->room_number;
		floor = GetFloor(wx, wy, wz, &room_number);
		h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
		c = GetCeiling(floor, wx, wy, wz);

		if(c > wy - 255 && h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT) {
			wy = (h + c) >> 1;
		} else if(h < wy + 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT) {
			wy = h - 255;
		} else if(c > wy - 255 && c < h && c != NO_HEIGHT && h != NO_HEIGHT) {
			wy = c + 255;
		}
	}

	room_number = ideal->room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx, wy, wz);

	if(h < wy || wy < c || h == NO_HEIGHT || c == NO_HEIGHT || c >= h) {
		return 1;
	}

	GetFloor(wx, wy, wz, &ideal->room_number);
	ideal->pos.x = wx;
	ideal->pos.y = wy;
	ideal->pos.z = wz;
	return 0;
}

void LaraTorch(PHD_VECTOR* Soffset, PHD_VECTOR* Eoffset, short yrot, long brightness) {
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	long x, y, z, sx, sy, sz, dx, dy, dz, falloff, counter, h, c, j;
	long offs[5];
	short room_number;

	counter = 0;
	falloff = 15;
	sx = Soffset->x;
	sy = Soffset->y;
	sz = Soffset->z;
	dx = (Eoffset->x - sx) >> 5;
	dy = (Eoffset->y - sy) >> 5;
	dz = (Eoffset->z - sz) >> 5;
	offs[0] = 0;
	offs[1] = -0x4000;
	offs[2] = -0x4001;
	offs[3] = 0x4000;
	offs[4] = 0x4001;

	for(int i = 0; i < MAX_DYNAMICS; i++) {
		if(counter) {
			counter--;

			brightness -= 7;

			if(brightness < 8) {
				break;
			}

			if(falloff < 31) {
				falloff += 2;
			}

			sx += dx;
			sy += dy;
			sz += dz;
			continue;
		}

		for(j = 0; j < 5; j++) {
			if(offs[j]) {
				x = sx + (falloff * (phd_sin(offs[j] + yrot) / 4) >> 5);

				if(offs[j] & 1) {
					y = sy - (falloff << 7);
				} else {
					y = sy + (falloff << 7);
				}

				z = sz + (falloff * (phd_cos(offs[j] + yrot) / 4) >> 5);
			} else {
				x = sx;
				y = sy;
				z = sz;
			}

			room_number = lara_item->room_number;
			floor = GetFloor(x, y, z, &room_number);
			h = GetHeight(floor, x, y, z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
			c = GetCeiling(floor, x, y, z);

			if(h == NO_HEIGHT || c == NO_HEIGHT || c >= h || y < c || h < y) {
				break;
			}
		}

		if(j < 5) {
			TriggerDynamic(sx, sy, sz, falloff, brightness, brightness, brightness >> 1);
			counter = 5;
		}

		if(counter) {
			counter--;
		}

		brightness -= 7;

		if(brightness < 8) {
			break;
		}

		if(falloff < 31) {
			falloff += 2;
		}

		sx += dx;
		sy += dy;
		sz += dz;
	}
}
void UpdateCameraElevation() {
	PHD_VECTOR pos;
	PHD_VECTOR pos1;

	if(camera.node != -1) {
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		if(camera.item != NULL) {
			GetJointAbsPosition(camera.item, &pos, camera.node);
		} else {
			GetLaraJointPos(&pos, camera.node);
		}

		pos1.x = 0;
		pos1.y = -256;
		pos1.z = 2048;
		if(camera.item != NULL) {
			GetJointAbsPosition(camera.item, &pos1, camera.node);
		} else {
			GetLaraJointPos(&pos1, camera.node);
		}

		pos.z = pos1.z - pos.z;
		pos.x = pos1.x - pos.x;
		camera.actual_angle = (short)(camera.target_angle + phd_atan(pos.z, pos.x));
	} else {
		camera.actual_angle = lara_item->pos.y_rot + camera.target_angle;
	}

	camera.actual_elevation += (camera.target_elevation - camera.actual_elevation) >> 3;
}


void ChaseCamera(ITEM_INFO* item) {
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	GAME_VECTOR ideal;
	GAME_VECTOR ideals[5];
	GAME_VECTOR temp[2];
	long distance, dx, dz, farthest, farthestnum, h, c, wx, wy, wz;
	short angle, room_number;

	if(!camera.target_elevation) {
		camera.target_elevation = -1820;
	}

	camera.target_elevation += item->pos.x_rot;
	UpdateCameraElevation();
	if(camera.target_elevation > 15470) {
		camera.target_elevation = 15470;
	} else if(camera.target_elevation < -15470) {
		camera.target_elevation = -15470;
	}

	distance = camera.target_distance * phd_cos(camera.target_elevation) >> W2V_SHIFT;
	wx = camera.target.pos.x;
	wy = camera.target.pos.y;
	wz = camera.target.pos.z;
	floor = GetFloor(wx, wy, wz, &camera.target.room_number);
	h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx, wy, wz);

	if(c + 16 > h - 16 && h != NO_HEIGHT && c != NO_HEIGHT) {
		camera.target.pos.y = (h + c) >> 1;
		camera.target_elevation = 0;
	} else if(wy > h - 16 && h != NO_HEIGHT) {
		camera.target.pos.y = h - 16;
		camera.target_elevation = 0;
	} else if(wy < c + 16 && c != NO_HEIGHT) {
		camera.target.pos.y = c + 16;
		camera.target_elevation = 0;
	}

	wx = camera.target.pos.x;
	wy = camera.target.pos.y;
	wz = camera.target.pos.z;
	GetFloor(wx, wy, wz, &camera.target.room_number);
	room_number = camera.target.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx, wy, wz);

	if(wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT) {
		camera.target.pos.x = last_target.pos.x;
		camera.target.pos.y = last_target.pos.y;
		camera.target.pos.z = last_target.pos.z;
		camera.target.room_number = last_target.room_number;
	} else {
		TargetSnaps = 0;
	}

	for(int i = 0; i < 5; i++) {
		ideals[i].pos.y = (camera.target_distance * phd_sin(camera.target_elevation) >> W2V_SHIFT) + camera.target.pos.y;
	}

	farthest = infinite_distance;
	farthestnum = 0;

	for(int lp = 0; lp < 5; lp++) {
		if(lp) {
			angle = (lp - 1) * 0x4000;
		} else {
			angle = camera.actual_angle;
		}

		ideals[lp].pos.x = camera.target.pos.x - (distance * phd_sin(angle) >> W2V_SHIFT);
		ideals[lp].pos.z = camera.target.pos.z - (distance * phd_cos(angle) >> W2V_SHIFT);
		ideals[lp].room_number = camera.target.room_number;

		if(mgLOS(&camera.target, &ideals[lp], 200)) {
			temp[0].pos.x = ideals[lp].pos.x;
			temp[0].pos.y = ideals[lp].pos.y;
			temp[0].pos.z = ideals[lp].pos.z;
			temp[0].room_number = ideals[lp].room_number;
			temp[1].pos.x = camera.pos.pos.x;
			temp[1].pos.y = camera.pos.pos.y;
			temp[1].pos.z = camera.pos.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if(mgLOS(&temp[0], &temp[1], 0) || !lp) {
				if(!lp) {
					farthestnum = 0;
					break;
				}

				dx = SQUARE(camera.pos.pos.x - ideals[lp].pos.x);
				dz = SQUARE(camera.pos.pos.z - ideals[lp].pos.z);
				dz += dx;

				if(dz < farthest) {
					farthest = dz;
					farthestnum = lp;
				}
			}
		} else if(!lp) {
			temp[0].pos.x = ideals[lp].pos.x;
			temp[0].pos.y = ideals[lp].pos.y;
			temp[0].pos.z = ideals[lp].pos.z;
			temp[0].room_number = ideals[lp].room_number;
			temp[1].pos.x = camera.pos.pos.x;
			temp[1].pos.y = camera.pos.pos.y;
			temp[1].pos.z = camera.pos.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if(mgLOS(&temp[0], &temp[1], 0)) {
				dx = SQUARE(camera.target.pos.x - ideals[lp].pos.x);
				dz = SQUARE(camera.target.pos.z - ideals[lp].pos.z);
				dz += dx;

				if(dz > 0x90000) {
					farthestnum = 0;
					break;
				}
			}
		}
	}


	ideal.pos.x = ideals[farthestnum].pos.x;
	ideal.pos.y = ideals[farthestnum].pos.y;
	ideal.pos.z = ideals[farthestnum].pos.z;
	ideal.room_number = ideals[farthestnum].room_number;
	CameraCollisionBounds(&ideal, 384, 1);

	if(camera.old_type == FIXED_CAMERA) {
		camera.speed = 1;
	}

	MoveCamera(&ideal, camera.speed);
}

void CombatCamera(ITEM_INFO* item) {
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	GAME_VECTOR ideal;
	GAME_VECTOR ideals[9];
	GAME_VECTOR temp[2];
	long distance, dx, dz, farthest, farthestnum, h, c, wx, wy, wz;
	short angle, room_number;

	camera.target.pos.x = item->pos.pos.x;
	camera.target.pos.z = item->pos.pos.z;

	if(lara.target_item != NO_ITEM) {
		camera.target_angle = lara.target_angles[0] + item->pos.y_rot;
		camera.target_elevation = lara.target_angles[1] + item->pos.x_rot;
	} else {
		camera.target_angle = lara.head_y_rot + lara.torso_y_rot + item->pos.y_rot;
		camera.target_elevation = lara.torso_x_rot + item->pos.x_rot + lara.head_x_rot - 2730;
	}

	wx = camera.target.pos.x;
	wy = camera.target.pos.y;
	wz = camera.target.pos.z;
	floor = GetFloor(wx, wy, wz, &camera.target.room_number);
	h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx, wy, wz);

	if(c + 64 > h - 64 && h != NO_HEIGHT && c != NO_HEIGHT) {
		camera.target.pos.y = (c + h) >> 1;
		camera.target_elevation = 0;
	} else if(camera.target.pos.y > h - 64 && h != NO_HEIGHT) {
		camera.target.pos.y = h - 64;
		camera.target_elevation = 0;
	} else if(camera.target.pos.y < c + 64 && c != NO_HEIGHT) {
		camera.target.pos.y = c + 64;
		camera.target_elevation = 0;
	}

	wx = camera.target.pos.x;
	wy = camera.target.pos.y;
	wz = camera.target.pos.z;
	GetFloor(wx, wy, wz, &camera.target.room_number);
	room_number = camera.target.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx, wy, wz);

	if(wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT) {
		camera.target.pos.x = last_target.pos.x;
		camera.target.pos.y = last_target.pos.y;
		camera.target.pos.z = last_target.pos.z;
		camera.target.room_number = last_target.room_number;
	}
	UpdateCameraElevation();

	camera.target_distance = 1536;
	distance = 1536 * phd_cos(camera.target_elevation) >> W2V_SHIFT;

	for(int i = 0; i < 5; i++) {
		ideals[i].pos.y = (1536 * phd_sin(camera.target_elevation) >> W2V_SHIFT) + camera.target.pos.y;
	}

	farthest = infinite_distance;
	farthestnum = 0;

	for(int i = 0; i < 5; i++) {
		if(i) {
			angle = (i - 1) << 14;
		} else {
			angle = camera.target_angle;
		}

		ideals[i].pos.x = camera.target.pos.x - ((distance * phd_sin(angle)) >> W2V_SHIFT);
		ideals[i].pos.z = camera.target.pos.z - ((distance * phd_cos(angle)) >> W2V_SHIFT);
		ideals[i].room_number = camera.target.room_number;

		if(mgLOS(&camera.target, &ideals[i], 200)) {
			temp[0].pos.x = ideals[i].pos.x;
			temp[0].pos.y = ideals[i].pos.y;
			temp[0].pos.z = ideals[i].pos.z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].pos.x = camera.pos.pos.x;
			temp[1].pos.y = camera.pos.pos.y;
			temp[1].pos.z = camera.pos.pos.z;
			temp[1].room_number = camera.pos.room_number;

			if(!i) {
				farthestnum = 0;
				break;
			}

			if(mgLOS(&temp[0], &temp[1], 0) || !i) {
				if(!i) {
					farthestnum = 0;
					break;
				}

				dx = SQUARE(camera.pos.pos.x - ideals[i].pos.x);
				dz = SQUARE(camera.pos.pos.z - ideals[i].pos.z);
				dz += dx;

				if(dz < farthest) {
					farthest = dz;
					farthestnum = i;
				}
			}
		} else if(!i) {
			temp[0].pos.x = ideals[i].pos.x;
			temp[0].pos.y = ideals[i].pos.y;
			temp[0].pos.z = ideals[i].pos.z;
			temp[0].room_number = ideals[i].room_number;
			temp[1].pos.x = camera.pos.pos.x;
			temp[1].pos.y = camera.pos.pos.y;
			temp[1].pos.z = camera.pos.pos.z;
			temp[1].room_number = camera.pos.room_number;
			dx = SQUARE(camera.target.pos.x - ideals[i].pos.x); // no mgLOS check here?
			dz = SQUARE(camera.target.pos.z - ideals[i].pos.z);
			dz += dx;

			if(dz > 0x90000) {
				farthestnum = 0;
				break;
			}
		}
	}

	ideal.pos.x = ideals[farthestnum].pos.x;
	ideal.pos.y = ideals[farthestnum].pos.y;
	ideal.pos.z = ideals[farthestnum].pos.z;
	ideal.room_number = ideals[farthestnum].room_number;
	CameraCollisionBounds(&ideal, 384, 1);

	if(camera.old_type == FIXED_CAMERA) {
		camera.speed = 1;
	}

	MoveCamera(&ideal, camera.speed);
}

void LookCamera(ITEM_INFO* item) {
	GAME_VECTOR ideal;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	PHD_VECTOR pos3;
	FLOOR_INFO* floor;
	long shake, dx, dy, dz, wx, wy, wz, h, c, rndval, lp;
	short room_number, room_number2, hxrot, txrot, hyrot, tyrot;

	hxrot = lara.head_x_rot;
	hyrot = lara.head_y_rot;
	txrot = lara.torso_x_rot;
	tyrot = lara.torso_y_rot;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	lara.head_x_rot <<= 1;
	lara.head_y_rot <<= 1;

	if(lara.head_x_rot > 10010) {
		lara.head_x_rot = 10010;
	} else if(lara.head_x_rot < -13650) {
		lara.head_x_rot = -13650;
	}

	if(lara.head_y_rot < -14560) {
		lara.head_y_rot = -14560;
	} else if(lara.head_y_rot > 14560) {
		lara.head_y_rot = 14560;
	}

	pos1.x = 0;
	pos1.y = 16;
	pos1.z = 64;
	GetLaraJointPos(&pos1, 8);
	room_number = lara_item->room_number;
	floor = GetFloor(pos1.x, pos1.y, pos1.z, &room_number);
	h = GetHeight(floor, pos1.x, pos1.y, pos1.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, pos1.x, pos1.y, pos1.z);

	if(h == NO_HEIGHT || c == NO_HEIGHT || c >= h || pos1.y > h || pos1.y < c) {
		pos1.x = 0;
		pos1.y = 16;
		pos1.z = 0;
		GetLaraJointPos(&pos1, 8);
		floor = GetFloor(pos1.x, pos1.y, pos1.z, &room_number);
		h = GetHeight(floor, pos1.x, pos1.y, pos1.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
		c = GetCeiling(floor, pos1.x, pos1.y, pos1.z);

		if(h == NO_HEIGHT || c == NO_HEIGHT || c >= h || pos1.y > h || pos1.y < c) {
			pos1.x = 0;
			pos1.y = 16;
			pos1.z = -64;
			GetLaraJointPos(&pos1, 8);
		}
	}

	pos2.x = 0;
	pos2.y = 0;
	pos2.z = -1024;
	GetLaraJointPos(&pos2, 8);
	pos3.x = 0;
	pos3.y = 0;
	pos3.z = 2048;
	GetLaraJointPos(&pos3, 8);
	wy = pos1.y;
	wx = pos1.x;
	wz = pos1.z;
	dx = (pos2.x - pos1.x) >> 3;
	dy = (pos2.y - pos1.y) >> 3;
	dz = (pos2.z - pos1.z) >> 3;
	room_number2 = lara_item->room_number;

	for(lp = 0; lp < 8; lp++) {
		room_number = room_number2;
		floor = GetFloor(wx, wy, wz, &room_number2);
		h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
		c = GetCeiling(floor, wx, wy, wz);

		if(h == NO_HEIGHT || c == NO_HEIGHT || c >= h || wy > h || wy < c) {
			break;
		}

		wz += dz;
		wx += dx;
		wy += dy;
	}

	if(lp) {
		wx -= dx;
		wy -= dy;
		wz -= dz;
	}

	ideal.pos.x = wx;
	ideal.pos.y = wy;
	ideal.pos.z = wz;
	ideal.room_number = room_number;

	if(old_cam.pos.x_rot == lara.head_x_rot && old_cam.pos.y_rot == lara.head_y_rot && old_cam.pos.pos.x == lara_item->pos.pos.x && old_cam.pos.pos.y == lara_item->pos.pos.y && old_cam.pos.pos.z == lara_item->pos.pos.z && old_cam.current_anim_state == lara_item->current_anim_state && old_cam.goal_anim_state == lara_item->goal_anim_state && camera.old_type == LOOK_CAMERA) {
		ideal.pos.x = static_lookcamp.pos.x;
		ideal.pos.y = static_lookcamp.pos.y;
		ideal.pos.z = static_lookcamp.pos.z;
		ideal.room_number = static_lookcamp.room_number;
		pos3.x = static_lookcamt.pos.x;
		pos3.y = static_lookcamt.pos.y;
		pos3.z = static_lookcamt.pos.z;
	} else {
		old_cam.pos.x_rot = lara.head_x_rot;
		old_cam.pos.y_rot = lara.head_y_rot;
		old_cam.pos.pos.x = lara_item->pos.pos.x;
		old_cam.pos.pos.y = lara_item->pos.pos.y;
		old_cam.pos.pos.z = lara_item->pos.pos.z;
		old_cam.current_anim_state = lara_item->current_anim_state;
		old_cam.goal_anim_state = lara_item->goal_anim_state;
		static_lookcamp.room_number = room_number;
		static_lookcamp.pos.x = wx;
		static_lookcamp.pos.y = wy;
		static_lookcamp.pos.z = wz;
		static_lookcamt.pos.x = pos3.x;
		static_lookcamt.pos.y = pos3.y;
		static_lookcamt.pos.z = pos3.z;
	}

	CameraCollisionBounds(&ideal, 224, 1);

	if(camera.old_type == FIXED_CAMERA) {
		camera.pos.pos.x = ideal.pos.x;
		camera.pos.pos.y = ideal.pos.y;
		camera.pos.pos.z = ideal.pos.z;
		camera.target.pos.x = pos3.x;
		camera.target.pos.y = pos3.y;
		camera.target.pos.z = pos3.z;
	} else {
		camera.pos.pos.x += (ideal.pos.x - camera.pos.pos.x) >> 2;
		camera.pos.pos.y += (ideal.pos.y - camera.pos.pos.y) >> 2;
		camera.pos.pos.z += (ideal.pos.z - camera.pos.pos.z) >> 2;
		camera.target.pos.x += (pos3.x - camera.target.pos.x) >> 2;
		camera.target.pos.y += (pos3.y - camera.target.pos.y) >> 2;
		camera.target.pos.z += (pos3.z - camera.target.pos.z) >> 2;
	}

	camera.target.room_number = lara_item->room_number;

	if(camera.bounce && camera.type == camera.old_type) {
		if(camera.bounce <= 0) {
			rndval = -camera.bounce;
			shake = -camera.bounce >> 1;
			camera.target.pos.x += GetRandomControl() % rndval - shake;
			camera.target.pos.y += GetRandomControl() % rndval - shake;
			camera.target.pos.z += GetRandomControl() % rndval - shake;
			camera.bounce += 5;
		} else {
			camera.pos.pos.y += camera.bounce;
			camera.target.pos.y += camera.bounce;
			camera.bounce = 0;
		}
	}

	GetFloor(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, &camera.pos.room_number);
	wx = camera.pos.pos.x;
	wy = camera.pos.pos.y;
	wz = camera.pos.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx, wy, wz);

	if(c > wy - 255 && h < wy + 255 && h > c && c != NO_HEIGHT && h != NO_HEIGHT) {
		camera.pos.pos.y = (h + c) >> 1;
	} else if(h < wy + 255 && h > c && c != NO_HEIGHT && h != NO_HEIGHT) {
		camera.pos.pos.y = h - 255;
	} else if(c > wy - 255 && h > c && c != NO_HEIGHT && h != NO_HEIGHT) {
		camera.pos.pos.y = c + 255;
	}

	wx = camera.pos.pos.x;
	wy = camera.pos.pos.y;
	wz = camera.pos.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx, wy, wz);

	if(wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT) {
		mgLOS(&camera.target, &camera.pos, 0);
	}

	wx = camera.pos.pos.x;
	wy = camera.pos.pos.y;
	wz = camera.pos.pos.z;
	room_number = camera.pos.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx, wy, wz);

	if(wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT) {
		camera.pos.pos.x = pos1.x;
		camera.pos.pos.y = pos1.y;
		camera.pos.pos.z = pos1.z;
		camera.pos.room_number = lara_item->room_number;
	}

	GetFloor(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, camera.target.pos.x, camera.target.pos.y, camera.target.pos.z, 0);

	if(camera.mike_at_lara) {
		camera.actual_elevation = lara.head_y_rot + lara.torso_y_rot + lara_item->pos.y_rot;
		camera.mike_pos.x = lara_item->pos.pos.x;
		camera.mike_pos.y = lara_item->pos.pos.y;
		camera.mike_pos.z = lara_item->pos.pos.z;
	} else {
		dx = camera.target.pos.x - camera.pos.pos.x;
		dz = camera.target.pos.z - camera.pos.pos.z;
		dx = phd_atan(dz, dx);
		camera.actual_elevation = (short)dx;
		camera.mike_pos.x = camera.pos.pos.x + (phd_sin(dx) * phd_persp >> W2V_SHIFT);
		camera.mike_pos.y = camera.pos.pos.y;
		camera.mike_pos.z = camera.pos.pos.z + (phd_cos(dx) * phd_persp >> W2V_SHIFT);
	}

	camera.old_type = camera.type;
	lara.head_x_rot = hxrot;
	lara.head_y_rot = hyrot;
	lara.torso_x_rot = txrot;
	lara.torso_y_rot = tyrot;
}

void FixedCamera() {
	OBJECT_VECTOR* fixed;
	GAME_VECTOR ideal;

	if(UseForcedFixedCamera) {
		ideal.pos.x = ForcedFixedCamera.pos.x;
		ideal.pos.y = ForcedFixedCamera.pos.y;
		ideal.pos.z = ForcedFixedCamera.pos.z;
		ideal.room_number = ForcedFixedCamera.room_number;
	} else {
		fixed = GetFixedCamera(currentLevel, camera.number);
		ideal.pos.x = fixed->x;
		ideal.pos.y = fixed->y;
		ideal.pos.z = fixed->z;
		ideal.room_number = fixed->data;
	}

	camera.fixed_camera = 1;
	MoveCamera(&ideal, 1);

	if(camera.timer) {
		camera.timer--;

		if(!camera.timer) {
			camera.timer = -1;
		}
	}
}

void BinocularCamera(ITEM_INFO* item) {
	FLOOR_INFO* floor;
	PHD_VECTOR pos1;
	PHD_VECTOR pos3;
	PHD_VECTOR Soffset;
	PHD_VECTOR Eoffset;
	short* ammo;
	long shake, speed, c, BinocStep, rndval, dx, dz;
	short room_number, hxrot, hyrot;
	char Fire;

	if(!LaserSight) {
		if(S_IsActionDown(inputImpl, IN_DRAW)) {
			ExittingBinos = 1;
		} else if(ExittingBinos) {
			ExittingBinos = 0;
			BinocularRange = 0;
			AlterFOV(14560);
			lara_item->mesh_bits = -1;
			lara.Busy = 0;
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.torso_x_rot = 0;
			bLaraTorch = 0;
			camera.type = BinocularOldCamera;
			return;
		}
	}

	lara_item->mesh_bits = 0;
	AlterFOV((short)(7 * (2080 - BinocularRange)));
	hxrot = lara.head_x_rot << 1;
	hyrot = lara.head_y_rot;

	if(hxrot > 13650) {
		hxrot = 13650;
	} else if(hxrot < -13650) {
		hxrot = -13650;
	}

	if(hyrot > 14560) {
		hyrot = 14560;
	} else if(hyrot < -14560) {
		hyrot = -14560;
	}

	hyrot += lara_item->pos.y_rot;
	pos1.x = lara_item->pos.pos.x;
	pos1.y = lara_item->pos.pos.y;
	pos1.z = lara_item->pos.pos.z;
	room_number = lara_item->room_number;
	floor = GetFloor(pos1.x, pos1.y, pos1.z, &room_number);
	c = GetCeiling(floor, pos1.x, pos1.y, pos1.z);

	if(c <= pos1.y - 768) {
		pos1.y -= 768;
	} else {
		pos1.y += 64;
	}

	speed = (20736 * phd_cos(hxrot)) >> W2V_SHIFT;
	pos3.x = pos1.x + (phd_sin(hyrot) * speed >> W2V_SHIFT);
	pos3.y = pos1.y - (phd_sin(hxrot) * 20736 >> W2V_SHIFT);
	pos3.z = pos1.z + (phd_cos(hyrot) * speed >> W2V_SHIFT);
	camera.pos.pos.x = pos1.x;
	camera.pos.pos.y = pos1.y;
	camera.pos.pos.z = pos1.z;
	camera.pos.room_number = room_number;

	if(camera.old_type == FIXED_CAMERA) {
		camera.target.pos.x = pos3.x;
		camera.target.pos.y = pos3.y;
		camera.target.pos.z = pos3.z;
	} else {
		camera.target.pos.x += (pos3.x - camera.target.pos.x) >> 2;
		camera.target.pos.y += (pos3.y - camera.target.pos.y) >> 2;
		camera.target.pos.z += (pos3.z - camera.target.pos.z) >> 2;
	}

	camera.target.room_number = lara_item->room_number;

	if(camera.bounce && camera.type == camera.old_type) {
		if(camera.bounce <= 0) {
			rndval = -camera.bounce;
			shake = rndval >> 1;
			camera.target.pos.x += (GetRandomControl() % rndval - shake) << 4;
			camera.target.pos.y += (GetRandomControl() % rndval - shake) << 4;
			camera.target.pos.z += (GetRandomControl() % rndval - shake) << 4;
			camera.bounce += 5;
		} else {
			camera.bounce = 0;
			camera.target.pos.y += camera.bounce;
		}
	}

	GetFloor(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, &camera.pos.room_number);
	phd_LookAt(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, camera.target.pos.x, camera.target.pos.y, camera.target.pos.z, 0);

	if(camera.mike_at_lara) {
		camera.actual_elevation = lara.head_y_rot + lara.torso_y_rot + lara_item->pos.y_rot;
		camera.mike_pos.x = lara_item->pos.pos.x;
		camera.mike_pos.y = lara_item->pos.pos.y;
		camera.mike_pos.z = lara_item->pos.pos.z;
	} else {
		dx = camera.target.pos.x - camera.pos.pos.x;
		dz = camera.target.pos.z - camera.pos.pos.z;
		dx = phd_atan(dz, dx);
		camera.actual_elevation = (short)dx;
		camera.mike_pos.x = camera.pos.pos.x + (phd_sin(dx) * phd_persp >> W2V_SHIFT);
		camera.mike_pos.y = camera.pos.pos.y;
		camera.mike_pos.z = camera.pos.pos.z + (phd_cos(dx) * phd_persp >> W2V_SHIFT);
	}

	BinocStep = S_IsActionDown(inputImpl, IN_WALK) ? 32 : 64;

	if(S_IsActionDown(inputImpl, IN_SPRINT)) {
		BinocularRange -= BinocStep;
	} else if(S_IsActionDown(inputImpl, IN_DUCK)) {
		BinocularRange += BinocStep;
	}

	if(BinocularRange < 128) {
		BinocularRange = 128;
	} else if(BinocularRange > 1536) {
		BinocularRange = 1536;
	}

	Soffset.x = camera.pos.pos.x;
	Soffset.y = camera.pos.pos.y;
	Soffset.z = camera.pos.pos.z;
	Eoffset.x = camera.target.pos.x;
	Eoffset.y = camera.target.pos.y;
	Eoffset.z = camera.target.pos.z;

	if(LaserSight) {
		ammo = get_current_ammo_pointer(lara.gun_type);

		if(S_IsActionDown(inputImpl, IN_ACTION) && !WeaponDelay && ammo[0]) {
			Fire = 1;

			if(lara.gun_type == WEAPON_REVOLVER) {
				WeaponDelay = 16;
				savegame.Game.AmmoUsed++;
			} else {
				WeaponDelay = 32;
			}

			if(ammo[0] != -1) {
				ammo[0]--;
			}
		} else {
			Fire = 0;
		}

		GetTargetOnLOS(&camera.pos, &camera.target, 1, Fire);
	} else {
		GetTargetOnLOS(&camera.pos, &camera.target, 0, 0);

		if(S_IsActionDown(inputImpl, IN_ACTION)) {
			LaraTorch(&Soffset, &Eoffset, lara.head_y_rot, 192);
		} else {
			bLaraTorch = 0;
		}
	}
}

void ConfirmCameraTargetPos() {
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	PHD_VECTOR pos;
	long wx, wy, wz, c, h;
	short room_number;

	pos.z = 0;
	pos.y = 0;
	pos.x = 0;
	if(camera.item == NULL) {
		GetJointAbsPosition(lara_item, &pos, LM_HEAD);
	}

	if(camera.node != -1) {
		camera.target.pos.x = pos.x;
		camera.target.pos.y = pos.y;
		camera.target.pos.z = pos.z;
	} else {
		camera.target.pos.x = lara_item->pos.pos.x;
		camera.target.pos.y = (camera.target.pos.y + pos.y) >> 1;
		camera.target.pos.z = lara_item->pos.pos.z;
	}

	wx = camera.target.pos.x;
	wy = camera.target.pos.y;
	wz = camera.target.pos.z;
	room_number = camera.target.room_number;
	floor = GetFloor(wx, wy, wz, &room_number);
	h = GetHeight(floor, wx, wy, wz, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	c = GetCeiling(floor, wx, wy, wz);

	if(wy < c || h < wy || h <= c || h == NO_HEIGHT || c == NO_HEIGHT) {
		camera.target.pos.x = pos.x;
		camera.target.pos.y = pos.y;
		camera.target.pos.z = pos.z;
	}
}

void CalculateCamera() {
	ITEM_INFO* item;
	OBJECT_VECTOR* fixed;
	PHD_VECTOR v;
	short bounds[6];
	long shift, fixed_camera, y, dx, dz;
	short gotit, change, tilt;

	if(BinocularRange) {
		BinocularOn = 1;
		BinocularCamera(lara_item);

		if(BinocularRange) {
			return;
		}
	}

	if(BinocularOn == 1) {
		BinocularOn = -8;
	}

	old_cam.t.z = camera.target.pos.z;
	old_cam.t.x = camera.target.pos.x;
	old_cam.t.y = camera.target.pos.y;

	if(UseForcedFixedCamera) {
		camera.type = FIXED_CAMERA;

		if(camera.old_type == FIXED_CAMERA) {
			camera.speed = 1;
		}
	}

	if(GetRoom(currentLevel, camera.pos.room_number)->flags & ROOM_UNDERWATER) {
		SoundEffect(SFX_UNDERWATER, 0, SFX_ALWAYS);

		if(!camera.underwater) {
			// empty func call here
			camera.underwater = 1;
		}
	} else if(camera.underwater) {
		// if unused var -> empty func call here
		camera.underwater = 0;
	}

	if(camera.type == CINEMATIC_CAMERA) {
		do_new_cutscene_camera();
		return;
	}

	item = camera.item;

	if(camera.item && (camera.type == FIXED_CAMERA || camera.type == HEAVY_CAMERA)) {
		fixed_camera = 1;
	} else {
		item = lara_item;
		fixed_camera = 0;
	}

	GetBoundsAccurate(item, bounds);

	if(fixed_camera) {
		y = item->pos.pos.y + (bounds[2] + bounds[3]) / 2;
	} else {
		y = item->pos.pos.y + bounds[3] + ((3 * (bounds[2] - bounds[3])) >> 2);
	}

	if(camera.item && !fixed_camera) {
		dx = camera.item->pos.pos.x - item->pos.pos.x;
		dz = camera.item->pos.pos.z - item->pos.pos.z;
		shift = phd_sqrt(SQUARE(dx) + SQUARE(dz));
		gotit = (short)(phd_atan(dz, dx) - item->pos.y_rot);
		gotit >>= 1;
		GetBoundsAccurate(camera.item, bounds);
		tilt = (short)phd_atan(shift, y - (bounds[2] + bounds[3]) / 2 - camera.item->pos.pos.y) >> 1;

		if(gotit > -9100 && gotit < 9100 && tilt > -15470 && tilt < 15470) {
			change = gotit - lara.head_y_rot;

			if(change > 728) {
				lara.head_y_rot += 728;
			} else if(change < -728) {
				lara.head_y_rot -= 728;
			} else {
				lara.head_y_rot = gotit;
			}

			lara.torso_y_rot = lara.head_y_rot;
			change = tilt - lara.head_x_rot;

			if(change > 728) {
				lara.head_x_rot += 728;
			} else if(change < -728) {
				lara.head_x_rot -= 728;
			} else {
				lara.head_x_rot = tilt;
			}

			lara.torso_x_rot = lara.head_x_rot;
			camera.type = LOOK_CAMERA;
			camera.item->looked_at = 1;
		}
	}

	if(camera.type != LOOK_CAMERA && camera.type != COMBAT_CAMERA) {
		last_target.pos.x = camera.target.pos.x;
		last_target.pos.y = camera.target.pos.y;
		last_target.pos.z = camera.target.pos.z;
		last_target.room_number = camera.target.room_number;
		camera.target.room_number = item->room_number;
		camera.target.pos.y = y;
		gotit = 0;

		if(camera.type != CHASE_CAMERA && camera.flags != 3) {
			fixed = GetFixedCamera(currentLevel, camera.number);

			if(fixed->flags & 2) {
				v.x = 0;
				v.y = 0;
				v.z = 0;
				GetLaraJointPos(&v, LM_TORSO);
				camera.target.pos.x = v.x;
				camera.target.pos.y = v.y;
				camera.target.pos.z = v.z;
				gotit = 1;
			}
		}

		if(!gotit) {
			shift = (bounds[0] + bounds[1] + bounds[4] + bounds[5]) >> 2;
			camera.target.pos.x = item->pos.pos.x + (shift * phd_sin(item->pos.y_rot) >> W2V_SHIFT);
			camera.target.pos.z = item->pos.pos.z + (shift * phd_cos(item->pos.y_rot) >> W2V_SHIFT);

			if(item->object_number == LARA) {
				ConfirmCameraTargetPos();
			}
		}

		if(fixed_camera != camera.fixed_camera) {
			camera.fixed_camera = 1;
			camera.speed = 1;
		} else {
			camera.fixed_camera = 0;

			if(camera.speed != 1 && camera.old_type != LOOK_CAMERA && BinocularOn >= 0) {
				if(TargetSnaps <= 8) {
					camera.target.pos.x = last_target.pos.x + ((camera.target.pos.x - last_target.pos.x) >> 2);
					camera.target.pos.y = last_target.pos.y + ((camera.target.pos.y - last_target.pos.y) >> 2);
					camera.target.pos.z = last_target.pos.z + ((camera.target.pos.z - last_target.pos.z) >> 2);
				} else {
					TargetSnaps = 0;
				}
			}
		}

		GetFloor(camera.target.pos.x, camera.target.pos.y, camera.target.pos.z, &camera.target.room_number);

		if(abs(last_target.pos.x - camera.target.pos.x) < 4 && abs(last_target.pos.y - camera.target.pos.y) < 4 && abs(last_target.pos.z - camera.target.pos.z) < 4) {
			camera.target.pos.x = last_target.pos.x;
			camera.target.pos.y = last_target.pos.y;
			camera.target.pos.z = last_target.pos.z;
		}

		if(camera.type != CHASE_CAMERA && camera.flags != 3) {
			FixedCamera();
		} else {
			ChaseCamera(item);
		}
	} else {
		if(camera.type == COMBAT_CAMERA) {
			last_target.pos.x = camera.target.pos.x;
			last_target.pos.y = camera.target.pos.y;
			last_target.pos.z = camera.target.pos.z;
			last_target.room_number = camera.target.room_number;
		}

		camera.target.room_number = item->room_number;

		if(camera.fixed_camera || BinocularOn < 0) {
			camera.target.pos.y = y;
			camera.speed = 1;
		} else {
			camera.target.pos.y += (y - camera.target.pos.y) >> 2;
			camera.speed = camera.type != LOOK_CAMERA ? 8 : 4;
		}

		camera.fixed_camera = 0;

		if(camera.type == LOOK_CAMERA) {
			LookCamera(item);
		} else {
			CombatCamera(item);
		}
	}

	camera.fixed_camera = fixed_camera;
	camera.last = camera.number;

	if(camera.type != HEAVY_CAMERA || camera.timer == -1) {
		camera.type = CHASE_CAMERA;
		camera.speed = 10;
		camera.number = -1;
		camera.last_item = camera.item;
		camera.item = 0;
		camera.target_elevation = 0;
		camera.target_angle = 0;
		camera.target_distance = 1536;
		camera.flags = 0;
		camera.node = -1;
	}
}
