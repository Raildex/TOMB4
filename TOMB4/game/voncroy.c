
#include "game/voncroy.h"
#include "distances.h"
#include "game/aibits.h"
#include "game/aiinfo.h"
#include "game/aiobject.h"
#include "game/animstruct.h"
#include "game/biteinfo.h"
#include "game/box.h"
#include "game/camera.h"
#include "game/control.h"
#include "game/creatureinfo.h"
#include "game/effects.h"
#include "game/heighttypes.h"
#include "game/inputbuttons.h"
#include "game/itemflags.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/lara.h"
#include "game/lara_states.h"
#include "game/larainfo.h"
#include "game/laramesh.h"
#include "game/levelinfo.h"
#include "game/lot.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "game/savegame.h"
#include "game/savegameinfo.h"
#include "game/spotcam.h"
#include "game/switch.h"
#include "game/tomb4fx.h"
#include "game/voncroycutdata.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/audio.h"
#include "specific/file.h"
#include "specific/function_stubs.h"
#include "specific/input.h"
#include <stdlib.h>
#include <string.h>


static BITE_INFO voncroy_hit = { 0, 35, 130, 18 };

static unsigned char VonCroyCutIndices[68] = // indices in VonCroyCutscenes depending on lara.locationPad
	{
		1, 2, 255, 0, 3, 255, 0, 4, 0, 0, 0, 0, 5, 6, 0, 0, 0, 255, 0, 0, 7, 0, 255, 255, 0, 8, 0, 255, 255, 255, 255, 255, 255, 255,
		9, 0, 10, 255, 255, 255, 255, 255, 255, 0, 255, 255, 0, 0, 11, 12, 255, 255, 255, 0, 255, 255, 0, 0, 13, 14, 255, 0, 0, 0, 0, 0, 0, 0
	};

static VonCroyCutData VonCroyCutscenes[15] = {
	{ { 256, -386, 256 }, { 0, 0, 0 }, 0x20050 },
	{ { 8845, 453, 83931 }, { 0, 0, 0 }, 0x40000 },
	{ { 0, -1024, 0 }, { 0, 0, 0 }, 0x20000 },
	{ { 17435, 2500, 61472 }, { 0, 0, 0 }, 0x40000 },
	{ { 30199, 1029, 51933 }, { 0, 0, 0 }, 0x60000 },
	{ { 38047, 468, 52008 }, { 27190, 1280, 60752 }, 0xC0000 },
	{ { 37130, 314, 61563 }, { 41883, -1291, 59413 }, 0xC0000 },
	{ { 55203, -3083, 53155 }, { 0, 0, 0 }, 0x40000 },
	{ { 60944, 601, 50535 }, { 62163, -432, 47405 }, 0xC0000 },
	{ { 94354, 366, 65718 }, { 92461, -432, 60717 }, 0xC0000 },
	{ { 94567, -2081, 63235 }, { 0, 0, 0 }, 0x60000 },
	{ { 79376, 219, 30345 }, { 0, 0, 0 }, 0x40000 },
	{ { 78067, -3375, 36470 }, { 0, 0, 0 }, 0x40000 },
	{ { 72648, 195, 41947 }, { 0, 0, 0 }, 0x40000 },
	{ { 66935, -3372, 40726 }, { 0, 0, 0 }, 0x40000 }
};

static short VonCroyCutTracks[64] = {
	31, 62, -1, 30, 24, -1, 17, 44, 1, 46, 3, 11, 10, 45, 13, 4, 39, -1, 67, 34, 61, -1, -1, -1, 70, 28, -1, -1, -1, -1, -1, -1,
	-1, -1, 68, 26, 43, -1, -1, -1, -1, -1, -1, 37, -1, -1, 36, 21, 25, 23, -1, -1, -1, 38, -1, -1, 36, 21, 25, 23, -1, -1, -1, -1
};

unsigned char VonCroyCutFlags[64]; // flags cut played
char bVoncroyCutScene;

static PHD_VECTOR actualCameraPos;
static PHD_VECTOR actualCameraTarget;
static long actualFOV;
static long actualRoomNumber;

void SetCutSceneCamera(ITEM_INFO* item) {
	VonCroyCutData* cut;
	long f;

	bVoncroyCutScene = 1;
	camera.old_type = FIXED_CAMERA;
	camera.type = CHASE_CAMERA;
	camera.speed = 1;
	CalculateCamera();
	actualCameraTarget.x = camera.target.pos.x;
	actualCameraTarget.y = camera.target.pos.y;
	actualCameraTarget.z = camera.target.pos.z;
	actualFOV = CurrentFov;
	actualRoomNumber = camera.pos.room_number;
	camera.underwater = 0;
	actualCameraPos.x = camera.pos.pos.x;
	actualCameraPos.y = camera.pos.pos.y;
	actualCameraPos.z = camera.pos.pos.z;
	cut = &VonCroyCutscenes[VonCroyCutIndices[lara.locationPad]];
	f = cut->f;

	if(f & 0xFFFF) {
		AlterFOV(182 * (f & 0xFFFF));
	}

	if(f & 0x40000) {
		camera.pos.pos.x = cut->CameraPos.x;
		camera.pos.pos.y = cut->CameraPos.y;
		camera.pos.pos.z = cut->CameraPos.z;
	} else {
		camera.pos.pos.x += cut->CameraPos.x;
		camera.pos.pos.y += cut->CameraPos.y;
		camera.pos.pos.z += cut->CameraPos.z;
	}

	if(f & 0x80000) {
		camera.target.pos.x = cut->CameraTarget.x;
		camera.target.pos.y = cut->CameraTarget.y;
		camera.target.pos.z = cut->CameraTarget.z;
	} else {
		camera.target.pos.x += cut->CameraTarget.x;
		camera.target.pos.y += cut->CameraTarget.y;
		camera.target.pos.z += cut->CameraTarget.z;
	}

	if(f & 0x20000) {
		camera.target.pos.x = item->pos.pos.x;
		camera.target.pos.y = item->pos.pos.y - 256;
		camera.target.pos.z = item->pos.pos.z;
	}

	if(IsRoomOutside(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z) == -2) {
		camera.pos.pos.x = actualCameraPos.x;
		camera.pos.pos.y = actualCameraPos.y;
		camera.pos.pos.z = actualCameraPos.z;
		camera.pos.room_number = (short)actualRoomNumber;
	} else {
		camera.pos.room_number = IsRoomOutsideNo;
	}

	phd_LookAt(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, camera.target.pos.x, camera.target.pos.y, camera.target.pos.z, 0);
}

void ClearCutSceneCamera() {
	camera.pos.pos.x = actualCameraPos.x;
	camera.pos.pos.y = actualCameraPos.y;
	camera.pos.pos.z = actualCameraPos.z;
	camera.target.pos.x = actualCameraTarget.x;
	camera.target.pos.y = actualCameraTarget.y;
	camera.target.pos.z = actualCameraTarget.z;
	camera.pos.room_number = (short)actualRoomNumber;
	camera.speed = 1;
	camera.old_type = FIXED_CAMERA;
	camera.type = CHASE_CAMERA;
	bVoncroyCutScene = 0;
	AlterFOV((short)actualFOV);
}

void GetAIEnemy(CREATURE_INFO* info, long tfl) {
	AIOBJECT* ai;

	for(int aiObjNum = 0; aiObjNum < GetNumAIObjects(currentLevel); aiObjNum++) {
		ai = GetAIObject(currentLevel, aiObjNum);

		if(ai->trigger_flags == tfl && ai->room_number != 255) {
			info->enemy = &info->ai_target;
			info->ai_target.object_number = ai->object_number;
			info->ai_target.room_number = ai->room_number;
			info->ai_target.pos.pos.x = ai->x;
			info->ai_target.pos.pos.y = ai->y;
			info->ai_target.pos.pos.z = ai->z;
			info->ai_target.pos.y_rot = ai->y_rot;
			info->ai_target.flags = ai->flags;
			info->ai_target.trigger_flags = ai->trigger_flags;
			info->ai_target.box_number = ai->box_number;

			if(!(info->ai_target.flags & IFL_TRIGGERED)) {
				info->ai_target.pos.pos.x += (256 * phd_sin(info->ai_target.pos.y_rot)) >> W2V_SHIFT;
				info->ai_target.pos.pos.z += (256 * phd_cos(info->ai_target.pos.y_rot)) >> W2V_SHIFT;
			}
		}
	}
}

void DoVonCroyCutscene(ITEM_INFO* item, CREATURE_INFO* info) {
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	long h;
	short ang, room_number;

	if(lara.locationPad != 8 && lara.locationPad != 15) {
		item->item_flags[3] = lara.locationPad;
	}

	if(lara.locationPad == 20 && lara_item->current_anim_state != AS_SURFTREAD) {
		return;
	}

	item->pos.z_rot = 0;

	if(item->item_flags[3] == 8 || item->item_flags[3] == 15) {
		if(lara_item->current_anim_state == AS_HANG || lara_item->current_anim_state == AS_HANGLEFT || lara_item->current_anim_state == AS_HANGRIGHT) {
			S_PlayTrack(musicImpl, VonCroyCutTracks[item->item_flags[3]], 0);
			VonCroyCutFlags[item->item_flags[3]] = 1;
			item->item_flags[2] = 2;
		}

		AnimateItem(item);
		return;
	}

	switch(item->trigger_flags) {
	case 0:
		SetFadeClip(24, 1);
		// empty func call here
		ScreenFading = 1;
		dScreenFade = 255;
		ScreenFade = 255;
		ScreenFadeSpeed = 16;
		ScreenFadeBack = 0;
		ScreenFadedOut = 1;
		DisableLaraControl();
		item->trigger_flags++;

		if(item->item_flags[3] == 14) {
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 43;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 27;
			item->goal_anim_state = 30;
		} else {
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 4;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 1;
			item->goal_anim_state = 1;
		}

		GetAIEnemy(info, lara.locationPad);
		item->pos.pos.x = info->enemy->pos.pos.x;
		item->pos.pos.y = info->enemy->pos.pos.y;
		item->pos.pos.z = info->enemy->pos.pos.z;
		ang = (short)phd_atan(lara_item->pos.pos.z - item->pos.pos.z, lara_item->pos.pos.x - item->pos.pos.x);

		if(item->item_flags[3] == 14 || item->item_flags[3] == 3) {
			item->pos.y_rot = info->enemy->pos.y_rot;
			info->LOT.is_jumping = 1;
			info->maximum_turn = 0;
		} else if(item->item_flags[3] == 43 || item->item_flags[3] == 53) {
			info->maximum_turn = 0;
			item->pos.y_rot = -0x6000;
		} else {
			item->pos.y_rot = ang;
		}

		IsRoomOutsideNo = -1;
		IsRoomOutside(item->pos.pos.x, item->pos.pos.y - 64, item->pos.pos.z);

		if(IsRoomOutsideNo != item->room_number && IsRoomOutsideNo != -1) {
			ItemNewRoom(GetItemNum(currentLevel, item), IsRoomOutsideNo);
		}

		lara_item->pos.y_rot = ang + 0x8000;

		if(lara_item->current_anim_state != AS_SURFTREAD) {
			room_number = lara_item->room_number;
			floor = GetFloor(lara_item->pos.pos.x, lara_item->pos.pos.y, lara_item->pos.pos.z, &room_number);
			h = GetHeight(floor, lara_item->pos.pos.x, lara_item->pos.pos.y, lara_item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
			lara_item->pos.pos.y = h;
			lara_item->anim_number = ANIM_STOP;
			lara_item->frame_number = GetAnim(currentLevel, ANIM_STOP)->frame_base;
			lara_item->current_anim_state = AS_STOP;
			lara_item->goal_anim_state = AS_STOP;
			lara_item->speed = 0;
			lara_item->fallspeed = 0;
			lara_item->gravity_status = 0;
		}

		break;

	case 1:

		if(ScreenFadedOut) {
			SetCutSceneCamera(item);
			ScreenFading = 0;
			SetScreenFadeIn(16);
			item->trigger_flags++;
			S_PlaySyncedTrack(musicImpl, VonCroyCutTracks[item->item_flags[3]]);
		}

		break;

	case 2:

		if(S_IsActionDown(inputImpl, IN_LOOK) && item->item_flags[3] != 43 && item->item_flags[3] != 53) {
			item->meshswap_meshbits &= ~0x8000;
			item->trigger_flags = 3;

			if(item->item_flags[3] != 14) {
				item->goal_anim_state = 1;
			}
		} else {
			if(/*XATrack != VonCroyCutTracks[item->item_flags[3]]*/ 1) {
				item->trigger_flags = 3;
			}

			if(item->current_anim_state == 1) {
				if(info->enemy && info->enemy->flags == 36) {
					item->goal_anim_state = 11;
				} else {
					item->goal_anim_state = GetRandomControl() % 3 + 8;
				}
			} else if(item->current_anim_state != 27) {
				if(info->enemy && info->enemy->flags == 36) {
					info->enemy->flags = 255;
				}

				item->goal_anim_state = 1;
			}
		}

		break;

	case 3:
		ClearCutSceneCamera();
		S_StopTrack(musicImpl);
		S_PlayTrack(musicImpl, CurrentAtmosphere, 1);
		IsAtmospherePlaying = 1;
		EnableLaraControl();
		SetFadeClip(0, 1);
		item->trigger_flags = 0;
		VonCroyCutFlags[item->item_flags[3]] = 1;
		ang = info->enemy->pos.y_rot - item->pos.y_rot;

		if(ang > 1024) {
			item->required_anim_state = 22;
		} else if(ang < -1024) {
			item->required_anim_state = 35;
		}

		item->goal_anim_state = 1;
		item->item_flags[2] = 2;
		break;
	};

	AnimateItem(item);

	if(item->current_anim_state == 11) {
		if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base + 32) {
			item->meshswap_meshbits |= 0x8000;
		} else if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base + 216) {
			item->meshswap_meshbits &= ~0x8000;
		}
	}
}

void InitialiseVoncroy(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);
	InitialiseCreature(item_number);
	item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 11;
	item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
	item->current_anim_state = 6;
	item->goal_anim_state = 6;
	item->meshswap_meshbits = 0x40080;
	memset(VonCroyCutFlags, 0, 64);
}

void VoncroyRaceControl(short item_number) {
	ITEM_INFO* item;
	ITEM_INFO* oEnemy;
	CREATURE_INFO* VonCroy;
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	AI_INFO info;
	long Xoffset, Zoffset, x, y, z, nearheight, midheight, farheight, dx, dz, distance, ahead, iAngle, h, c;
	short tilt, angle, torso_x, torso_y, head, room_number, jump_ahead, long_jump_ahead, ifl3;
	short* meshpp = GetMesh(currentLevel, GetObjectInfo(currentLevel, VON_CROY)->mesh_index + 42);
	static long talk = 0;

	if(!CreatureActive(item_number)) {
		return;
	}

	item = GetItem(currentLevel, item_number);
	VonCroy = (CREATURE_INFO*)item->data;
	tilt = 0;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;
	ifl3 = 0;
	room_number = item->room_number;
	Xoffset = 808 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
	Zoffset = 808 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
	x = item->pos.pos.x + Xoffset;
	y = item->pos.pos.y;
	z = item->pos.pos.z + Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	nearheight = GetHeight(floor, x, y, z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	midheight = GetHeight(floor, x, y, z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	farheight = GetHeight(floor, x, y, z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

	if(y >= nearheight - 384 || y >= midheight + 256 || y <= midheight - 256) {
		jump_ahead = 0;
	} else {
		jump_ahead = 1;
	}

	if(y >= nearheight - 384 || y >= midheight - 384 || y >= farheight + 256 || y <= farheight - 256) {
		long_jump_ahead = 0;
	} else {
		long_jump_ahead = 1;
	}

	GetAITarget(VonCroy);
	oEnemy = VonCroy->enemy;

	if(item->anim_number == GetObjectInfo(currentLevel, VON_CROY)->anim_index + 36 || item->anim_number == GetObjectInfo(currentLevel, VON_CROY)->anim_index + 52) {
		item->pos.pos.x += Xoffset;
		item->pos.pos.z += Zoffset;
		room_number = item->room_number;
		GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &item->room_number);

		if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base + 1) {
			CreateZone(item);
		}

		CreatureAIInfo(item, &info);
		item->room_number = room_number;
		item->pos.pos.z -= Zoffset;
		item->pos.pos.x -= Xoffset;
	} else {
		CreatureAIInfo(item, &info);
	}

	GetCreatureMood(item, &info, 1);
	CreatureMood(item, &info, 1);

	if(VonCroy->enemy == lara_item) {
		iAngle = info.angle;
		distance = info.distance;
	} else {
		dx = lara_item->pos.pos.x - item->pos.pos.x;
		dz = lara_item->pos.pos.z - item->pos.pos.z;
		iAngle = phd_atan(dz, dx) - item->pos.y_rot;

		if(iAngle > -0x4000 && iAngle < 0x4000) {
			ahead = 1;
		} else {
			ahead = 0;
		}

		distance = SQUARE(dx) + SQUARE(dz);
	}

	angle = CreatureTurn(item, VonCroy->maximum_turn);

	if(bUseSpotCam /* && XATrack == 80 */) {
		talk++;

		if((talk > 0 && talk < 565) || (talk > 705 && talk < 927)) {
			*GetMeshPointer(currentLevel, GetObjectInfo(currentLevel, VON_CROY)->mesh_index + 42) = GetMesh(currentLevel, GetObjectInfo(currentLevel, ACTOR1_SPEECH_HEAD1 + (GetRandomControl() & 1))->mesh_index + 42);
		} else {
			*GetMeshPointer(currentLevel, GetObjectInfo(currentLevel, VON_CROY)->mesh_index + 42) = meshpp;
		}

		if(talk > 580 && talk < 693) {
			lara.mesh_ptrs[LM_HEAD] = GetMesh(currentLevel, GetObjectInfo(currentLevel, (GetRandomControl() & 3) + LARA_SPEECH_HEAD1)->mesh_index + 2 * LM_HEAD);
		} else {
			lara.mesh_ptrs[LM_HEAD] = GetMesh(currentLevel, GetObjectInfo(currentLevel, LARA_SKIN)->mesh_index + 2 * LM_HEAD);
		}
	} else {
		talk = 0;
		lara.mesh_ptrs[LM_HEAD] = GetMesh(currentLevel, GetObjectInfo(currentLevel, LARA_SKIN)->mesh_index + 2 * LM_HEAD);
		*GetMeshPointer(currentLevel, GetObjectInfo(currentLevel, VON_CROY)->mesh_index + 42) = meshpp;
	}

	switch(item->current_anim_state) {
	case 1:
		VonCroy->LOT.is_jumping = 0;
		VonCroy->LOT.is_monkeying = 0;
		VonCroy->flags = 0;
		VonCroy->maximum_turn = 0;
		head = info.angle >> 1;

		if(info.ahead) {
			torso_x = info.x_angle >> 1;
			torso_y = info.angle >> 1;
		}

		if(lara.location < item->item_flags[3] /* || XATrack == 80 && (XAFlag == 6 || XAFlag == 5)*/) {
			item->goal_anim_state = 1;
			break;
		}

		if(VonCroy->reached_goal) {
			if(oEnemy && oEnemy->flags && (distance < 0x900000 || !GetObjectInfo(currentLevel, BAT)->loaded) || lara.location > item->item_flags[3]) {
				if(item->item_flags[2] != 6) {
					item->item_flags[2] = 0;
				}

				switch(oEnemy->flags) {
				case 0:
				case 32:
					ifl3 = -1;
					break;

				case 2:
					item->current_anim_state = 29;
					item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 37;
					item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
					item->pos = oEnemy->pos;
					ifl3 = 1;
					break;

				case 4:
					item->current_anim_state = 26;
					item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 36;
					item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
					VonCroy->LOT.is_jumping = 1;
					item->pos = oEnemy->pos;
					ifl3 = 1;
					break;

				case 6:

					if(lara.location > item->item_flags[3]) {
						ifl3 = 1;
					} else if(item->meshswap_meshbits & 0x40080) {
						item->goal_anim_state = 6;
					} else {
						item->goal_anim_state = 31;
					}

					break;

				case 8:

					if(lara.location > item->item_flags[3]) {
						ifl3 = 1;
					} else {
						item->goal_anim_state = 20;
					}

					break;

				case 10:

					if(lara.location > item->item_flags[3]) {
						ifl3 = 1;
					} else {
						item->goal_anim_state = 7;
					}

					break;

				case 34:

					if(lara.location > item->item_flags[3]) {
						ifl3 = 2;
					} else {
						item->goal_anim_state = 32;
					}

					break;

				case 36:

					if(lara.location > item->item_flags[3]) {
						ifl3 = 1;
					} else {
						item->goal_anim_state = 11;
					}

					break;

				case 40:

					if(item->item_flags[2] == 6) {
						item->goal_anim_state = 3;
					} else {
						item->goal_anim_state = 34;
						item->pos = oEnemy->pos;
					}

					break;
				}
			} else {
				ifl3 = 1;
			}
		} else if(jump_ahead || long_jump_ahead) {
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 22;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 15;

			if(long_jump_ahead) {
				item->goal_anim_state = 16;
			} else {
				item->goal_anim_state = 15;
			}

			VonCroy->LOT.is_jumping = 1;
		} else if(VonCroy->monkey_ahead) {
			floor = GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);
			h = GetHeight(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
			c = GetCeiling(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z);

			if(c == h - 1536) {
				item->goal_anim_state = 4;
			} else {
				item->goal_anim_state = 2;
			}
		} else if(oEnemy != lara_item || info.distance > 0x64000) {
			item->goal_anim_state = 2;
		}

		break;

	case 2:
		VonCroy->LOT.is_jumping = 0;
		VonCroy->LOT.is_monkeying = 0;
		VonCroy->maximum_turn = 1092;

		if(ahead) {
			head = (short)iAngle;
		} else if(info.ahead) {
			head = info.angle;
		}

		if(!savegame.Level.Timer) {
			savegame.Level.Timer = 1;
		}

		if(lara.location < item->item_flags[3]) {
			item->goal_anim_state = 1;
		} else if(jump_ahead || long_jump_ahead) {
			VonCroy->maximum_turn = 0;
			item->goal_anim_state = 1;
		} else if(VonCroy->monkey_ahead) {
			item->goal_anim_state = 1;
		} else if(!VonCroy->reached_goal) {
			if(info.distance < 0x64000 && oEnemy->flags != 32) {
				item->goal_anim_state = 1;
			} else if(info.distance > 0x900000) {
				item->goal_anim_state = 3;
			}
		} else if(oEnemy->flags == 32) {
			ifl3 = -1;
		} else {
			item->goal_anim_state = 1;
		}

		break;

	case 3:

		if(info.ahead) {
			head = info.angle;
		}

		if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base) {
			VonCroy->LOT.is_jumping = 0;
			VonCroy->maximum_turn = 1456;
		}

		tilt = angle >> 1;

		if(item->item_flags[2] == 6) {
			VonCroy->maximum_turn = 0;
			item->goal_anim_state = 16;
		} else if(lara.location < item->item_flags[3] || jump_ahead) {
			item->goal_anim_state = 1;
		} else if(long_jump_ahead) {
			VonCroy->maximum_turn = 0;
			item->goal_anim_state = 16;
		} else if(VonCroy->monkey_ahead) {
			item->goal_anim_state = 1;
		} else if(VonCroy->reached_goal) {
			if(oEnemy->flags == 32) {
				ifl3 = -1;
			} else if(info.distance >= 0x4000) {
				item->goal_anim_state = 1;
			} else if(oEnemy->flags == 40) {
				VonCroy->maximum_turn = 0;
				item->pos.y_rot = oEnemy->pos.y_rot;
				item->goal_anim_state = 16;
				item->item_flags[2] = 6;
			}
		} else if(info.distance < 0x64000 && oEnemy->flags != 32 && oEnemy->flags != 40) {
			item->goal_anim_state = 1;
		}

		break;

	case 4:
		VonCroy->maximum_turn = 0;

		if(item->box_number == VonCroy->LOT.target_box || !VonCroy->monkey_ahead) {
			floor = GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);
			h = GetHeight(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
			c = GetCeiling(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z);

			if(c == h - 1536) {
				item->goal_anim_state = 1;
			}
		} else {
			item->goal_anim_state = 5;
		}

		break;

	case 5:
		VonCroy->LOT.is_jumping = 1;
		VonCroy->LOT.is_monkeying = 1;
		VonCroy->maximum_turn = 1092;

		if(item->box_number == VonCroy->LOT.target_box || !VonCroy->monkey_ahead) {
			floor = GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);
			h = GetHeight(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
			c = GetCeiling(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z);

			if(c == h - 1536) {
				item->goal_anim_state = 4;
			}
		}

		break;

	case 6:

		if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base + 28) {
			if(item->meshswap_meshbits & 0x40080) {
				item->meshswap_meshbits &= ~0x40080;
			} else {
				item->meshswap_meshbits |= 0x40080;
			}
		}

		break;

	case 15:

		if(item->anim_number == GetObjectInfo(currentLevel, VON_CROY)->anim_index + 23) {
			item->goal_anim_state = 3;
		}

		break;

	case 16:

		if(item->anim_number == GetObjectInfo(currentLevel, VON_CROY)->anim_index + 25 || item->frame_number > GetAnim(currentLevel, item->anim_number)->frame_base + 7) {
			VonCroy->LOT.is_jumping = 1;
		} else if(jump_ahead) {
			item->goal_anim_state = 15;
		} else if(!GetObjectInfo(currentLevel, BAT)->loaded) {
			item->goal_anim_state = 3;
		}

		if(item->item_flags[2] == 6) {
			item->goal_anim_state = 33;
		}

		break;

	case 20:

		if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base) {
			item->pos = oEnemy->pos;
		} else if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base + 120) {
			ifl3 = -1;
		}

		break;

	case 22:
	case 35:
		VonCroy->maximum_turn = 0;

		if(item->item_flags[2]) {
			CreatureYRot(&item->pos, oEnemy->pos.y_rot - item->pos.y_rot, 512);
		} else {
			CreatureYRot(&item->pos, (short)iAngle, 512);
		}

		break;

	case 31:

		if(info.ahead) {
			head = info.angle >> 1;
			torso_y = info.angle >> 1;
			torso_x = info.x_angle >> 1;
		}

		VonCroy->maximum_turn = 0;

		if(abs(info.angle) < 1092) {
			item->pos.y_rot += info.angle;
		} else if(info.angle < 0) {
			item->pos.y_rot -= 1092;
		} else {
			item->pos.y_rot += 1092;
		}

		if(oEnemy && oEnemy->flags == 6 && item->frame_number > GetAnim(currentLevel, item->anim_number)->frame_base + 21) {
			ifl3 = -1;
		} else if(!VonCroy->flags && oEnemy) {
			if(item->frame_number > GetAnim(currentLevel, item->anim_number)->frame_base + 15 && item->frame_number < GetAnim(currentLevel, item->anim_number)->frame_base + 26) {
				if(abs(oEnemy->pos.pos.x - item->pos.pos.x) < 512 && abs(oEnemy->pos.pos.y - item->pos.pos.y) <= 512 && abs(oEnemy->pos.pos.z - item->pos.pos.z) < 512) {
					oEnemy->hit_points -= 20;
					oEnemy->hit_status = 1;
					VonCroy->flags = 1;
					CreatureEffectT(item, &voncroy_hit, 8, -1, DoBloodSplat);
				}
			}
		}

		break;

	case 33:

		if(item->anim_number == GetObjectInfo(currentLevel, VON_CROY)->anim_index + 52 && item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base) {
			ifl3 = 1;
		}

		item->goal_anim_state = 2;
		item->item_flags[2] = 0;
		break;

	case 34:
		item->item_flags[2] = 6;
		break;

	case 36:
	case 37:
		VonCroy->maximum_turn = 0;
		MoveCreature3DPos(&item->pos, &oEnemy->pos, 8, oEnemy->pos.y_rot - item->pos.y_rot, 512);
		break;
	}

	if(ifl3 == -1 && oEnemy) {
		room_number = oEnemy->room_number;
		floor = GetFloor(oEnemy->pos.pos.x, oEnemy->pos.pos.y, oEnemy->pos.pos.z, &room_number);
		GetHeight(floor, oEnemy->pos.pos.x, oEnemy->pos.pos.y, oEnemy->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
		TestTriggers(trigger_index, 1, 0);
		ifl3 = 1;
	}

	if(ifl3) {
		VonCroy->reached_goal = 0;
		VonCroy->enemy = NULL;
		item->item_flags[3] += ifl3;
		item->ai_bits = FOLLOW;
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureJoint(item, 3, torso_x);

	if(item->current_anim_state >= 15 || item->current_anim_state == 5) {
		CreatureAnimation(item_number, angle, 0);
	} else {
		switch(CreatureVault(item_number, angle, 2, 260)) {
		case -4:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 35;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 25;
			break;

		case -3:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 41;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 24;
			break;

		case -2:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 42;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 23;
			break;

		case 2:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 29;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 19;
			break;

		case 3:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 28;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 18;
			break;

		case 4:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 27;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 17;
			break;
		}
	}
}

void VoncroyControl(short item_number) {
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	ITEM_INFO* target;
	ITEM_INFO* candidate;
	CREATURE_INFO* VonCroy;
	CREATURE_INFO* baddie;
	FLOOR_INFO* floor;
	height_types height_type;
	long tiltxoff, tiltzoff, OnObject;
	static AI_INFO VonCroyAI;
	static AI_INFO VonCroyLaraAI;
	long Xoffset, Zoffset, x, y, z, nearheight, midheight, farheight, dx, dz, dist, max_dist, h, c;
	short tilt, angle, torso_x, torso_y, head, room_number, jump_ahead, long_jump_ahead, ifl3;

	if(!CreatureActive(item_number)) {
		return;
	}

	item = GetItem(currentLevel, item_number);
	VonCroy = (CREATURE_INFO*)item->data;

	tilt = 0;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;
	ifl3 = 0;

	room_number = item->room_number;
	Xoffset = 808 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
	Zoffset = 808 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;

	x = item->pos.pos.x + Xoffset;
	y = item->pos.pos.y;
	z = item->pos.pos.z + Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	nearheight = GetHeight(floor, x, y, z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	midheight = GetHeight(floor, x, y, z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

	room_number = item->room_number;
	x += Xoffset;
	z += Zoffset;
	floor = GetFloor(x, y, z, &room_number);
	farheight = GetHeight(floor, x, y, z, &height_type, &tiltxoff, &tiltzoff, &OnObject);

	jump_ahead = y < nearheight - 384 && y < midheight + 256 && y > midheight - 256;
	long_jump_ahead = y < nearheight - 384 && y < midheight - 384 && y < farheight + 256 && y > farheight - 256;

	item->ai_bits = FOLLOW;
	GetAITarget(VonCroy);
	target = 0;

	if(lara.location <= item->item_flags[3]) {
		max_dist = infinite_distance;

		for(int i = 0; i < 5; i++) {
			baddie = &baddie_slots[i];

			if(baddie->item_num == NO_ITEM || baddie->item_num == item_number) {
				continue;
			}

			candidate = GetItem(currentLevel, baddie->item_num);

			if(candidate->object_number != VON_CROY) {
				dx = candidate->pos.pos.x - item->pos.pos.x;
				dz = candidate->pos.pos.z - item->pos.pos.z;
				dist = SQUARE(dx) + SQUARE(dz);

				if(abs(dx) <= 5120 && abs(dz) <= 5120 && dist < max_dist) {
					VonCroy->reached_goal = 0;
					target = candidate;
					max_dist = dist;
					item->item_flags[2] = 0;
				}
			}
		}
	}

	enemy = VonCroy->enemy;

	if(target) {
		VonCroy->enemy = target;
	}

	if(item->anim_number == GetObjectInfo(currentLevel, VON_CROY)->anim_index + 36 || item->anim_number == GetObjectInfo(currentLevel, VON_CROY)->anim_index + 52) {
		item->pos.pos.x += Xoffset;
		item->pos.pos.z += Zoffset;
		room_number = item->room_number;
		GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &item->room_number);

		if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base + 1) {
			CreateZone(item);
		}

		CreatureAIInfo(item, &VonCroyAI);
		item->room_number = room_number;
		item->pos.pos.z -= Zoffset;
		item->pos.pos.x -= Xoffset;
	} else {
		CreatureAIInfo(item, &VonCroyAI);
	}

	GetCreatureMood(item, &VonCroyAI, 1);
	CreatureMood(item, &VonCroyAI, 1);

	if(VonCroy->enemy == lara_item) {
		memcpy(&VonCroyLaraAI, &VonCroyAI, sizeof(VonCroyLaraAI));
	} else {
		dx = lara_item->pos.pos.x - item->pos.pos.x;
		dz = lara_item->pos.pos.z - item->pos.pos.z;
		VonCroyLaraAI.angle = (short)(phd_atan(dz, dx) - item->pos.y_rot);
		VonCroyLaraAI.ahead = VonCroyLaraAI.angle > -0x4000 && VonCroyLaraAI.angle < 0x4000;
		VonCroyLaraAI.enemy_facing = VonCroyLaraAI.angle + 0x8000 - lara_item->pos.y_rot;

		if(dx > 32000 || dx < -32000 || dz > 32000 || dz < -32000) {
			VonCroyLaraAI.distance = infinite_distance;
		} else {
			VonCroyLaraAI.distance = SQUARE(dx) + SQUARE(dz);
		}

		dx = abs(dx);
		dz = abs(dz);

		if(dx > dz) {
			VonCroyLaraAI.x_angle = (short)phd_atan(dx + (dz >> 1), item->pos.pos.y - lara_item->pos.pos.y);
		} else {
			VonCroyLaraAI.x_angle = (short)phd_atan(dz + (dx >> 1), item->pos.pos.y - lara_item->pos.pos.y);
		}
	}

	VonCroyLaraAI.bite = VonCroyLaraAI.angle > -0x1800 && VonCroyLaraAI.angle < 0x1800 && VonCroyLaraAI.distance < 0x100000;
	angle = CreatureTurn(item, VonCroy->maximum_turn);

	if(target) {
		VonCroy->enemy = enemy;
		enemy = target;
	}

	if(item->item_flags[3] == 43 && savegame.Game.Secrets > 7) {
		VonCroy->reached_goal = 0;
		VonCroy->enemy = NULL;
		item->ai_bits = FOLLOW;
		item->item_flags[3] = 53;
		lara.location = 53;
	}

	if((lara.locationPad == 9 || lara.locationPad == 10) && item->item_flags[3] == 11) {
		lara.locationPad = 11;
	} else if(lara.locationPad == 10 && item->item_flags[3] == 12 && (item->item_flags[0] || lara_item->anim_number == GetObjectInfo(currentLevel, LARA)->anim_index + 90 && lara_item->frame_number == GetAnim(currentLevel, lara_item->anim_number)->frame_end)) {
		lara.locationPad = (char)item->item_flags[3];
		item->item_flags[0] = 1;
	} else if(lara.locationPad == 43 && (item->item_flags[3] == 43 || item->item_flags[3] == 53)) {
		lara.locationPad = (char)item->item_flags[3];
		lara.location = lara.locationPad;
	} else if(lara.location == 43 && (item->item_flags[3] == 44 || item->item_flags[3] == 54 || item->item_flags[3] == 44 || item->item_flags[3] == 54)) {
		lara.location = (char)item->item_flags[3];
	}

	if(!VonCroyCutFlags[item->item_flags[3]]) {
		if(VonCroy->reached_goal && item->item_flags[3] == lara.locationPad && VonCroyCutTracks[item->item_flags[3]] != -1 || item->trigger_flags > 0 || lara.locationPad >= item->item_flags[3] && !VonCroyCutFlags[lara.locationPad] && VonCroyCutTracks[lara.locationPad] != -1) {
			CreatureJoint(item, 0, VonCroyLaraAI.angle >> 1);
			CreatureJoint(item, 1, VonCroyLaraAI.x_angle >> 1);
			CreatureJoint(item, 2, VonCroyLaraAI.angle >> 1);
			CreatureJoint(item, 3, VonCroyLaraAI.x_angle >> 1);
			DoVonCroyCutscene(item, VonCroy);
			return;
		}
	}

	switch(item->current_anim_state) {
	case 1:
		VonCroy->LOT.is_jumping = 0;
		VonCroy->LOT.is_monkeying = 0;
		VonCroy->flags = 0;
		VonCroy->maximum_turn = 0;
		head = VonCroyAI.angle >> 1;

		if(VonCroyAI.ahead) {
			torso_x = VonCroyAI.x_angle >> 1;
			torso_y = VonCroyAI.angle >> 1;
		}

		if(item->required_anim_state) {
			item->goal_anim_state = item->required_anim_state;
		} else if(item->item_flags[2] == 2) {
			if(enemy->pos.y_rot - item->pos.y_rot < -1024) {
				item->goal_anim_state = 35;
			} else if(enemy->pos.y_rot - item->pos.y_rot > 1024) {
				item->goal_anim_state = 22;
			} else {
				item->item_flags[2] = 0;

				if(!enemy->flags) {
					ifl3 = 1;
				}
			}
		} else if(lara.location < item->item_flags[3] && VonCroy->reached_goal) {
			item->goal_anim_state = 1;
		} else if(target && VonCroyAI.distance < 0x900000 && item->meshswap_meshbits & 0x40080) {
			item->goal_anim_state = 6;
		} else if(target && VonCroyAI.distance < 0x100000) {
			if(VonCroyAI.bite) {
				item->goal_anim_state = 31;
			} else if(enemy->hit_points > 0 && VonCroyAI.ahead) {
				if(abs(enemy->pos.pos.y + 512 - item->pos.pos.y) < 512) {
					item->goal_anim_state = 21;
				}
			}
		} else if(target && enemy != lara_item && VonCroyAI.distance > 0x64000) {
			item->goal_anim_state = 2;
		} else if(VonCroy->reached_goal) {
			if(VonCroyAI.distance > 0x4000 && enemy->flags && item->item_flags[2] != 6) {
				VonCroy->maximum_turn = 0;

				if(VonCroyAI.ahead) {
					item->required_anim_state = 36;
				} else {
					item->required_anim_state = 37;
				}

				break;
			}

			if(lara.location > item->item_flags[3] || enemy && enemy->flags && (lara.locationPad == item->item_flags[3] || VonCroyCutTracks[item->item_flags[3]] == -1 && lara.location == item->item_flags[3] && VonCroyLaraAI.distance < 0x900000)) {
				if(enemy->flags > 32) {
					switch(enemy->flags) {
					case 34:

						if(lara.location > item->item_flags[3]) {
							ifl3 = 2;
						} else {
							item->goal_anim_state = 32;
						}

						break;

					case 36:

						if(lara.location > item->item_flags[3]) {
							ifl3 = 1;
						} else {
							item->goal_anim_state = 1;
						}

						break;

					case 40:

						if(item->item_flags[2] == 6) {
							item->goal_anim_state = 3;
						} else {
							item->goal_anim_state = 34;
							item->pos = enemy->pos;
						}

						break;

					case 48:
						ifl3 = -1;
						break;

					case 255:
						ifl3 = 1;
						break;
					}
				} else if(enemy->flags == 32) {
					ifl3 = -1;
				} else {
					switch(enemy->flags) {
					case 0:
						ifl3 = -1;
						break;

					case 2:
						item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 37;
						item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
						item->current_anim_state = 29;
						item->pos = enemy->pos;
						ifl3 = 1;
						break;

					case 4:
						item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 36;
						item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
						item->current_anim_state = 26;
						VonCroy->LOT.is_jumping = 1;
						item->pos = enemy->pos;
						ifl3 = 1;
						break;

					case 8:
						item->goal_anim_state = 20;
						break;

					case 10:
						item->goal_anim_state = 7;
						break;

					case 12:
						VonCroy->maximum_turn = 0;
						item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 22;
						item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
						item->current_anim_state = 15;

						if(long_jump_ahead) {
							item->goal_anim_state = 16;
						} else {
							item->goal_anim_state = 15;
						}

						VonCroy->LOT.is_jumping = 1;
						item->pos = enemy->pos;
						ifl3 = 1;
						break;
					}
				}
			} else if(enemy && enemy->flags && VonCroyLaraAI.distance >= 0x900000) {
				item->goal_anim_state = 1;
			} else if(item->item_flags[2]) {
				if(item->item_flags[2] != 1) {
					ifl3 = 1;
				} else if(GetRandomControl() & 0xF) {
					item->item_flags[2] = 0;
				} else if(VonCroyLaraAI.distance >= 0x900000) {
					item->goal_anim_state = 13;
				} else {
					item->goal_anim_state = 14;
				}
			} else if(VonCroyLaraAI.angle > 1024) {
				item->goal_anim_state = 35;
			} else if(VonCroyLaraAI.angle < -1024) {
				item->goal_anim_state = 22;
			} else {
				item->item_flags[2] = 1;
			}
		} else if(VonCroyLaraAI.bite) {
			item->goal_anim_state = 1;
		} else if(VonCroy->monkey_ahead) {
			floor = GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);
			h = GetHeight(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
			c = GetCeiling(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z);

			if(c == h - 1536) {
				if(item->meshswap_meshbits & 0x40080) {
					item->goal_anim_state = 4;
				} else {
					item->goal_anim_state = 6;
				}
			} else {
				item->goal_anim_state = 2;
			}
		} else if(target && VonCroyAI.distance < 0x900000 && item->meshswap_meshbits & 0x40080) {
			item->goal_anim_state = 6;
		} else if(target && VonCroyAI.distance < 0x100000) {
			if(VonCroyAI.bite) {
				item->goal_anim_state = 31;
			} else if(enemy->hit_points > 0 && VonCroyAI.ahead) {
				if(abs(enemy->pos.pos.y + 512 - item->pos.pos.y) < 512) {
					item->goal_anim_state = 21;
				}
			}
		} else if(VonCroyAI.distance > 0x64000 && VonCroyLaraAI.distance < 0x1900000 || lara.location >= item->item_flags[3]) {
			item->goal_anim_state = 2;
		}

		break;

	case 2:
		VonCroy->LOT.is_jumping = 0;
		VonCroy->LOT.is_monkeying = 0;
		VonCroy->maximum_turn = 1092;

		if(VonCroyLaraAI.ahead) {
			head = VonCroyLaraAI.angle;
		} else if(VonCroyAI.ahead) {
			head = VonCroyAI.angle;
		}

		if(item->required_anim_state) {
			item->goal_anim_state = item->required_anim_state;
		} else if(lara.location < item->item_flags[3] && VonCroyLaraAI.distance > 0x1900000 || VonCroyLaraAI.bite) {
			item->goal_anim_state = 1;
		} else if(VonCroy->monkey_ahead) {
			item->goal_anim_state = 1;
		} else if(VonCroy->reached_goal) {
			if(enemy->flags == 32) {
				ifl3 = -1;
			} else {
				item->goal_anim_state = 1;
			}
		} else if(!target || VonCroyAI.distance >= 0x200000 && (item->meshswap_meshbits & 0x40080 || VonCroyAI.distance >= 0x900000)) {
			if(VonCroyAI.distance < 0x64000 && enemy->flags != 32) {
				item->goal_anim_state = 1;
			} else if(VonCroyAI.distance > 0x900000 && lara.location >= item->item_flags[3]) {
				item->goal_anim_state = 3;
			}
		} else {
			item->goal_anim_state = 1;
		}

		break;

	case 3:

		if(VonCroyAI.ahead) {
			head = VonCroyAI.angle;
		}

		if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base) {
			VonCroy->LOT.is_jumping = 0;
			VonCroy->maximum_turn = 1456;
		}

		tilt = angle >> 1;

		if(item->item_flags[2] == 6) {
			VonCroy->maximum_turn = 0;
			item->goal_anim_state = 16;
		} else if(lara.location < item->item_flags[3] || jump_ahead || VonCroyLaraAI.bite) {
			item->goal_anim_state = 1;
		} else if(VonCroy->monkey_ahead) {
			item->goal_anim_state = 1;
		} else if(VonCroy->reached_goal) {
			if(enemy->flags == 32) {
				ifl3 = -1;
			} else if(VonCroyAI.distance >= 512) {
				item->goal_anim_state = 1;
			} else if(enemy->flags == 40) {
				VonCroy->maximum_turn = 0;
				item->pos.y_rot = enemy->pos.y_rot;
				item->goal_anim_state = 16;
				item->item_flags[2] = 6;
			}
		} else if(VonCroyAI.distance < 0x64000 && enemy->flags != 32 && enemy->flags != 40) {
			item->goal_anim_state = 1;
		}

		break;

	case 4:
		VonCroy->maximum_turn = 0;

		if(item->box_number == VonCroy->LOT.target_box || !VonCroy->monkey_ahead) {
			floor = GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);
			h = GetHeight(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
			c = GetCeiling(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z);

			if(c == h - 1536) {
				item->goal_anim_state = 1;
			}
		} else {
			item->goal_anim_state = 5;
		}

		break;

	case 5:
		VonCroy->LOT.is_jumping = 1;
		VonCroy->LOT.is_monkeying = 1;
		VonCroy->maximum_turn = 1092;

		if(item->box_number == VonCroy->LOT.target_box || !VonCroy->monkey_ahead) {
			floor = GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);
			h = GetHeight(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &height_type, &tiltxoff, &tiltzoff, &OnObject);
			c = GetCeiling(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z);

			if(c == h - 1536) {
				item->goal_anim_state = 4;
			}
		}

		break;

	case 6:

		if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base + 28) {
			if(item->meshswap_meshbits & 0x40080) {
				item->meshswap_meshbits &= ~0x40080;
			} else {
				item->meshswap_meshbits |= 0x40080;
			}
		}

		break;

	case 7:

		if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base) {
			item->pos = enemy->pos;

			if(item->item_flags[3] == 6) {
				VonCroy->maximum_turn = 0;
				item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 22;
				item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
				item->current_anim_state = 15;
				item->goal_anim_state = 16;
				VonCroy->LOT.is_jumping = 1;
			}

			ifl3 = 1;
		}

		break;

	case 16:

		if(item->anim_number == GetObjectInfo(currentLevel, VON_CROY)->anim_index + 25 || item->frame_number > GetAnim(currentLevel, item->anim_number)->frame_base + 5) {
			VonCroy->LOT.is_jumping = 1;
		} else if(jump_ahead) {
			item->goal_anim_state = 15;
		}

		if(item->item_flags[2] == 6) {
			item->goal_anim_state = 33;
		}

		break;

	case 20:

		if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base) {
			item->pos = enemy->pos;
		} else if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base + 120) {
			ifl3 = -1;
		}

		break;

	case 21:

		if(VonCroyAI.ahead) {
			torso_y = VonCroyAI.angle >> 1;
			head = VonCroyAI.angle >> 1;
			torso_x = VonCroyAI.x_angle >> 1;
		}

		VonCroy->maximum_turn = 0;
		CreatureYRot(&item->pos, VonCroyAI.angle, 1092);

		if(!VonCroy->flags && enemy && item->frame_number > GetAnim(currentLevel, item->anim_number)->frame_base + 20 && item->frame_number < GetAnim(currentLevel, item->anim_number)->frame_base + 45) {
			if(abs(enemy->pos.pos.x - item->pos.pos.x) < 512 && abs(enemy->pos.pos.y + 768 - item->pos.pos.y) <= 512 && abs(enemy->pos.pos.z - item->pos.pos.z) < 512) {
				enemy->hit_points -= 40;

				if(enemy->hit_points <= 0) {
					item->ai_bits = FOLLOW;
				}

				enemy->hit_status = 1;
				VonCroy->flags = 1;
				CreatureEffectT(item, &voncroy_hit, 2, -1, DoBloodSplat);
			}
		}

		break;

	case 22:
	case 35:
		VonCroy->maximum_turn = 0;

		if(item->item_flags[2]) {
			CreatureYRot(&item->pos, enemy->pos.y_rot - item->pos.y_rot, 512);
		} else {
			CreatureYRot(&item->pos, VonCroyLaraAI.angle, 512);
		}

		break;

	case 27:
		VonCroy->LOT.is_jumping = 1;
		VonCroy->maximum_turn = 0;

		if(VonCroy->reached_goal) {
			item->goal_anim_state = 30;
			ifl3 = 1;
		} else {
			item->goal_anim_state = 28;
		}

		break;

	case 28:
		VonCroy->LOT.is_jumping = 1;
		VonCroy->maximum_turn = 0;
		break;

	case 31:

		if(VonCroyAI.ahead) {
			torso_y = VonCroyAI.angle >> 1;
			head = VonCroyAI.angle >> 1;
			torso_x = VonCroyAI.x_angle >> 1;
		}

		VonCroy->maximum_turn = 0;
		CreatureYRot(&item->pos, VonCroyAI.angle, 1092);

		if(enemy && enemy->flags == 6 && item->frame_number > GetAnim(currentLevel, item->anim_number)->frame_base + 21) {
			ifl3 = -1;
			VonCroy->flags = 1;
		} else if(!VonCroy->flags && enemy && item->frame_number > GetAnim(currentLevel, item->anim_number)->frame_base + 15 && item->frame_number < GetAnim(currentLevel, item->anim_number)->frame_base + 26) {
			if(abs(enemy->pos.pos.x - item->pos.pos.x) < 512 && abs(enemy->pos.pos.y - item->pos.pos.y) <= 512 && abs(enemy->pos.pos.z - item->pos.pos.z) < 512) {
				enemy->hit_points -= 20;

				if(enemy->hit_points <= 0) {
					item->ai_bits = FOLLOW;
				}

				enemy->hit_status = 1;
				VonCroy->flags = 1;
				CreatureEffectT(item, &voncroy_hit, 8, -1, DoBloodSplat);
			}
		}

		break;

	case 32:

		if(VonCroyAI.ahead) {
			torso_y = VonCroyAI.angle >> 1;
			head = VonCroyAI.angle >> 1;
			torso_x = VonCroyAI.x_angle;
		}

		VonCroy->maximum_turn = 0;
		CreatureYRot(&item->pos, VonCroyAI.angle, 1092);

		if(item->anim_number == GetObjectInfo(currentLevel, VON_CROY)->anim_index + 47) {
			if(item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base) {
				ifl3 = 1;
			}
		} else if(!(GetRandomControl() & 0x1F)) {
			ifl3 = 1;
			item->goal_anim_state = 1;
		}

		break;

	case 33:

		if(item->anim_number == GetObjectInfo(currentLevel, VON_CROY)->anim_index + 52 && item->frame_number == GetAnim(currentLevel, item->anim_number)->frame_base) {
			ifl3 = 1;
		}

		item->goal_anim_state = 2;
		item->required_anim_state = 3;
		item->item_flags[2] = 0;
		break;

	case 34:
		item->item_flags[2] = 6;
		break;

	case 36:
	case 37:
		VonCroy->maximum_turn = 0;
		MoveCreature3DPos(&item->pos, &enemy->pos, 15, enemy->pos.y_rot - item->pos.y_rot, 512);
		break;
	}

	if(ifl3 == -1) {
		enemy = &VonCroy->ai_target;
		TestTriggersAtXYZ(VonCroy->ai_target.pos.pos.x, VonCroy->ai_target.pos.pos.y, VonCroy->ai_target.pos.pos.z, VonCroy->ai_target.room_number, 1, 0);
		ifl3 = 1;
	}

	if(ifl3) {
		VonCroy->reached_goal = 0;
		VonCroy->enemy = NULL;
		item->ai_bits = FOLLOW;
		item->item_flags[3] += ifl3;
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureJoint(item, 3, torso_x);

	if(item->current_anim_state >= 15 || item->current_anim_state == 5) {
		CreatureAnimation(item_number, angle, 0);
	} else {
		switch(CreatureVault(item_number, angle, 2, 260)) {
		case -4:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 35;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 25;
			break;

		case -3:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 41;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 24;
			break;

		case -2:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 42;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 23;
			break;

		case 2:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 29;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 19;
			break;

		case 3:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 28;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 18;
			break;

		case 4:
			VonCroy->maximum_turn = 0;
			item->anim_number = GetObjectInfo(currentLevel, VON_CROY)->anim_index + 27;
			item->frame_number = GetAnim(currentLevel, item->anim_number)->frame_base;
			item->current_anim_state = 17;
			break;
		}
	}
}
