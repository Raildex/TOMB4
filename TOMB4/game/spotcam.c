
#include "game/spotcam.h"
#include "game/camera.h"
#include "game/control.h"
#include "game/gameflow.h"
#include "game/inputbuttons.h"
#include "game/iteminfo.h"
#include "game/lara.h"
#include "game/larainfo.h"
#include "game/levelinfo.h"
#include "game/switch.h"
#include "game/tomb4fx.h"
#include "specific/3dmath.h"
#include "specific/input.h"
#include "specific/windows/dxshell.h"
#include <dinput.h>


long bTrackCamInit = 0;
long bUseSpotCam = 0;
long bDisableLaraControl = 0;
short LastSequence;
short CurrentFov;

static PHD_VECTOR LaraFixedPosition;
static PHD_VECTOR InitialCameraPosition;
static PHD_VECTOR InitialCameraTarget;

static long camera_xposition[18];
static long camera_yposition[18];
static long camera_zposition[18];
static long camera_xtarget[18];
static long camera_ytarget[18];
static long camera_ztarget[18];
static long camera_roll[18];
static long camera_fov[18];
static long camera_speed[18];
static long CameraFade;
static long LaraHealth;
static long LaraAir;
static long spline_to_camera;
static long spline_from_camera;
static long bCheckTrigger = 0;
static long current_spline_position;
static short current_sequence;
static short current_spline_camera;
static short current_camera_cnt;
static short first_camera;
static short last_camera;
static short InitialCameraRoom;
static short LastFov;
static short spotcam_timer;
static short spotcam_loopcnt;
static unsigned char SpotRemap[8];
static unsigned char CameraCnt[8];

void SetSplineData(long num, long cam) {
	SPOTCAM* spotcam;
	ITEM_INFO* item;

	spotcam = GetSpotCam(currentLevel, cam);

	camera_xposition[num] = spotcam->x;
	camera_yposition[num] = spotcam->y;
	camera_zposition[num] = spotcam->z;
	camera_xtarget[num] = spotcam->tx;
	camera_ytarget[num] = spotcam->ty;
	camera_ztarget[num] = spotcam->tz;

	if(spotcam->flags & 0x10) {
		camera_xtarget[num] = LaraFixedPosition.x;
		camera_ytarget[num] = LaraFixedPosition.y;
		camera_ztarget[num] = LaraFixedPosition.z;
	} else {
		camera_xtarget[num] = spotcam->tx;
		camera_ytarget[num] = spotcam->ty;
		camera_ztarget[num] = spotcam->tz;
	}

	if(spotcam->flags & 2) {
		item = GetItem(currentLevel, spotcam->timer);

		if(item) {
			camera_xtarget[num] = item->pos.pos.x;
			camera_ytarget[num] = item->pos.pos.y;
			camera_ztarget[num] = item->pos.pos.z;
		}
	}

	camera_roll[num] = spotcam->roll;
	camera_speed[num] = spotcam->speed;
	camera_fov[num] = spotcam->fov;
}

void InitialiseSpotCam(short Sequence) {
	SPOTCAM* s;
	long next_spline_camera, cunt;

	if(bTrackCamInit && Sequence == LastSequence) {
		bTrackCamInit = 0;
		return;
	}

	LastFov = CurrentFov;
	CameraFade = -1;
	LastSequence = Sequence;
	bTrackCamInit = 0;
	spotcam_timer = 0;
	spotcam_loopcnt = 0;
	EnableLaraControl();
	LaraHealth = lara_item->hit_points;
	LaraAir = lara.air;
	InitialCameraTarget.x = camera.target.pos.x;
	InitialCameraTarget.y = camera.target.pos.y;
	InitialCameraTarget.z = camera.target.pos.z;
	InitialCameraPosition.x = camera.pos.pos.x;
	InitialCameraPosition.y = camera.pos.pos.y;
	InitialCameraPosition.z = camera.pos.pos.z;
	InitialCameraRoom = camera.pos.room_number;
	LaraFixedPosition.x = lara_item->pos.pos.x;
	LaraFixedPosition.y = lara_item->pos.pos.y;
	LaraFixedPosition.z = lara_item->pos.pos.z;
	current_sequence = Sequence;
	current_spline_camera = 0;

	for(int i = 0; i < SpotRemap[Sequence]; i++) {
		current_spline_camera += CameraCnt[i];
	}

	current_camera_cnt = CameraCnt[SpotRemap[Sequence]];
	first_camera = current_spline_camera;
	last_camera = first_camera + current_camera_cnt - 1;
	current_spline_position = 0;
	spline_to_camera = 0;
	s = GetSpotCam(currentLevel, current_spline_camera);

	if(s->flags & 0x400 || gfGameMode == 1) {
		DisableLaraControl();

		if(gfGameMode != 1) {
			SetFadeClip(16, 1);
		}
	}

	if(s->flags & 8) {
		spline_from_camera = 0;
		cunt = 0;
		SetSplineData(cunt, first_camera); // INLINED
		cunt++;
		next_spline_camera = first_camera;

		for(int i = 0; i < current_camera_cnt; i++) {
			SetSplineData(cunt, next_spline_camera); // INLINED
			cunt++;
			next_spline_camera++;
		}

		SetSplineData(cunt, last_camera); // INLINED
	} else if(s->flags & 1) {
		spline_from_camera = 0;
		cunt = 0;
		next_spline_camera = current_spline_camera;
		SetSplineData(cunt, next_spline_camera); // INLINED
		cunt++;

		while(cunt < 4) {
			if(next_spline_camera > last_camera) {
				next_spline_camera = first_camera;
			}

			SetSplineData(cunt, next_spline_camera);
			cunt++;
			next_spline_camera++;
		}

		current_spline_camera++;

		if(current_spline_camera > last_camera) {
			current_spline_camera = first_camera;
		}

		if(s->flags & 0x4000) {
			bCheckTrigger = 1;
		}
	} else {
		spline_from_camera = 1;
		camera_xtarget[0] = InitialCameraTarget.x;
		camera_ytarget[0] = InitialCameraTarget.y;
		camera_ztarget[0] = InitialCameraTarget.z;
		camera_xposition[0] = InitialCameraPosition.x;
		camera_yposition[0] = InitialCameraPosition.y;
		camera_zposition[0] = InitialCameraPosition.z;
		camera_roll[0] = 0;
		camera_fov[0] = CurrentFov;
		camera_xposition[1] = camera_xposition[0];
		camera_yposition[1] = camera_yposition[0];
		camera_zposition[1] = camera_zposition[0];
		camera_xtarget[1] = camera_xtarget[0];
		camera_ytarget[1] = camera_ytarget[0];
		camera_ztarget[1] = camera_ztarget[0];
		camera_roll[1] = camera_roll[0];
		camera_fov[1] = camera_fov[0];
		camera_speed[1] = camera_speed[0];
		cunt = current_spline_camera;
		SetSplineData(2, cunt);
		cunt++;

		if(cunt > last_camera) {
			cunt = first_camera;
		}

		SetSplineData(3, cunt);
	}
}

void EnableLaraControl() {
	bDisableLaraControl = 0;
}
void DisableLaraControl() {
	bDisableLaraControl = 1;
	S_ClearInput(inputImpl);
}

void InitSpotCamSequences() {
	unsigned char s, cc, ce;

	bTrackCamInit = 0;

	if(GetNumSpotcams(currentLevel)) {
		ce = 0;
		s = GetSpotCam(currentLevel, 0)->sequence;
		cc = 1;

		for(int i = 1; i < GetNumSpotcams(currentLevel); i++) {
			if(GetSpotCam(currentLevel, i)->sequence == s) {
				cc++;
			} else {
				CameraCnt[ce] = cc;
				SpotRemap[s] = ce;
				s = GetSpotCam(currentLevel, i)->sequence;
				cc = 1;
				ce++;
			}
		}

		CameraCnt[ce] = cc;
		SpotRemap[s] = ce;
	}
}

long Spline(long x, long* knots, long nk) {
	long* k;
	long span, c1, c2;

	span = x * (nk - 3) >> 16;

	if(span >= nk - 3) {
		span = nk - 4;
	}

	k = &knots[span];
	x = x * (nk - 3) - span * 65536;
	c1 = (k[1] >> 1) - (k[2] >> 1) - k[2] + k[1] + (k[3] >> 1) + ((-k[0] - 1) >> 1);
	c2 = 2 * k[2] - 2 * k[1] - (k[1] >> 1) - (k[3] >> 1) + k[0];
	return ((long long)x * (((long long)x * (((long long)x * c1 >> 16) + c2) >> 16) + (k[2] >> 1) + ((-k[0] - 1) >> 1)) >> 16) + k[1];
}

void CalculateSpotCams() {
	SPOTCAM* FirstCam;
	SPOTCAM* CurrentCam;
	ITEM_INFO* item;
	CAMERA_INFO backup;
	camera_type ctype;
	long cpx, cpy, cpz, ctx, cty, ctz, cspeed, cfov, croll, next_spline_camera, cunt;
	long cs, cp, clen, tlen, cx, cy, cz, lx, ly, lz, sp;
	static long bFirstLook = 0;
	short spline_cnt;

	if(bDisableLaraControl) {
		lara_item->hit_points = (short)LaraHealth;
		lara.air = (short)LaraAir;
	}

	FirstCam = GetSpotCam(currentLevel, first_camera);
	sp = 0;

	if(FirstCam->flags & 8) {
		spline_cnt = current_camera_cnt + 2;
	} else {
		spline_cnt = 4;
	}

	cpx = Spline(current_spline_position, camera_xposition, spline_cnt);
	cpy = Spline(current_spline_position, camera_yposition, spline_cnt);
	cpz = Spline(current_spline_position, camera_zposition, spline_cnt);
	ctx = Spline(current_spline_position, camera_xtarget, spline_cnt);
	cty = Spline(current_spline_position, camera_ytarget, spline_cnt);
	ctz = Spline(current_spline_position, camera_ztarget, spline_cnt);
	cspeed = Spline(current_spline_position, camera_speed, spline_cnt);
	croll = Spline(current_spline_position, camera_roll, spline_cnt);
	cfov = Spline(current_spline_position, camera_fov, spline_cnt);
	CurrentCam = GetSpotCam(currentLevel, current_spline_camera);

	if(CurrentCam->flags & 0x1000 && CameraFade != current_spline_camera) {
		CameraFade = current_spline_camera;
	}

	if(CurrentCam->flags & 0x2000 && CameraFade != current_spline_camera) {
		CameraFade = current_spline_camera;
	}

	if(FirstCam->flags & 8) {
		cp = 0;
		cs = 0x2000;
		lx = lara_item->pos.pos.x;
		ly = lara_item->pos.pos.y;
		lz = lara_item->pos.pos.z;

		for(int i = 0; i < 8; i++) {
			tlen = 0x10000;

			for(int j = 0; j < 8; j++) {
				cx = Spline(sp, camera_xposition, spline_cnt) - lx;
				cy = Spline(sp, camera_yposition, spline_cnt) - ly;
				cz = Spline(sp, camera_zposition, spline_cnt) - lz;
				clen = phd_sqrt(SQUARE(cx) + SQUARE(cy) + SQUARE(cz));

				if(clen <= tlen) {
					cp = sp;
					tlen = clen;
				}

				sp += cs;

				if(sp > 0x10000) {
					break;
				}
			}

			cs >>= 1;
			sp = cp - (cs << 1);

			if(sp < 0) {
				sp = 0;
			}
		}

		current_spline_position += (cp - current_spline_position) >> 5;

		if(FirstCam->flags & 1) {
			if(abs(cp - current_spline_position) > 0x8000) {
				current_spline_position = cp;
			}
		}

		if(cp < 0) {
			current_spline_position = 0;
		} else if(cp > 0x10000) {
			current_spline_position = 0x10000;
		}
	} else if(!spotcam_timer) {
		current_spline_position += cspeed;
	}

	if(keymap[DIK_ESCAPE] && gfCurrentLevel) {
		current_spline_position = 0x10000;
	}

	if(!(S_IsActionDown(inputImpl, IN_LOOK))) {
		bFirstLook = 0;
	}

	if(FirstCam->flags & 0x200 || !(S_IsActionDown(inputImpl, IN_LOOK)) || gfGameMode == 1) {
		camera.pos.pos.x = cpx;
		camera.pos.pos.y = cpy;
		camera.pos.pos.z = cpz;

		if(FirstCam->flags & 0x28) {
			camera.target.pos.x = lara_item->pos.pos.x;
			camera.target.pos.y = lara_item->pos.pos.y;
			camera.target.pos.z = lara_item->pos.pos.z;
		} else {
			camera.target.pos.x = ctx;
			camera.target.pos.y = cty;
			camera.target.pos.z = ctz;
		}

		if(CurrentCam->flags & 2) {
			item = GetItem(currentLevel, GetSpotCam(currentLevel, current_spline_camera)->timer);

			if(item) {
				camera.target.pos.x = item->pos.pos.x;
				camera.target.pos.y = item->pos.pos.y;
				camera.target.pos.z = item->pos.pos.z;
			}
		}

		if(IsRoomOutside(cpx, cpy, cpz) == -2) {
			camera.pos.room_number = GetSpotCam(currentLevel, current_spline_camera)->room_number;
			GetFloor(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, &camera.pos.room_number);
		} else {
			camera.pos.room_number = IsRoomOutsideNo;
		}

		AlterFOV((short)cfov);
		phd_LookAt(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, camera.target.pos.x, camera.target.pos.y, camera.target.pos.z, (short)croll);

		if(bCheckTrigger) {
			ctype = camera.type;
			camera.type = HEAVY_CAMERA;

			if(gfCurrentLevel) {
				TestTriggersAtXYZ(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, camera.pos.room_number, 1, 0);
			} else {
				TestTriggersAtXYZ(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, camera.pos.room_number, 0, 0);
				TestTriggersAtXYZ(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, camera.pos.room_number, 1, 0);
			}

			camera.type = ctype;
			bCheckTrigger = 0;
		}

		if(FirstCam->flags & 8) {
			bTrackCamInit = 1;
		} else if(current_spline_position > 0x10000 - cspeed) {
			if(CurrentCam->flags & 0x4000) {
				bCheckTrigger = 1;
			}

			if(CurrentCam->flags & 0x100) {
				if(spotcam_timer) {
					spotcam_timer--;
				} else {
					spotcam_timer = GetSpotCam(currentLevel, current_spline_camera)->timer >> 4;
				}
			}

			if(!spotcam_timer) {
				current_spline_position = 0;

				if(current_spline_camera == first_camera) {
					next_spline_camera = last_camera;
				} else {
					next_spline_camera = current_spline_camera - 1;
				}

				cunt = 1;

				if(spline_from_camera) {
					spline_from_camera = 0;
					next_spline_camera = first_camera - 1;
				} else {
					if(CurrentCam->flags & 0x800) {
						EnableLaraControl();
					}

					if(CurrentCam->flags & 0x400) {
						SetFadeClip(16, 1);
						DisableLaraControl();
					}

					cunt = 0;

					if(CurrentCam->flags & 0x80) {
						next_spline_camera = first_camera + (GetSpotCam(currentLevel, current_spline_camera)->timer & 0xF);
						current_spline_camera = (short)next_spline_camera;
						SetSplineData(cunt, next_spline_camera); // INLINED
						cunt = 1;
					}

					SetSplineData(cunt, next_spline_camera); // INLINED
					cunt++;
				}

				next_spline_camera++;

				while(cunt < 4) {
					if(FirstCam->flags & 4) {
						if(next_spline_camera > last_camera) {
							next_spline_camera = first_camera;
						}
					} else if(next_spline_camera > last_camera) {
						next_spline_camera = last_camera;
					}

					SetSplineData(cunt, next_spline_camera); // INLINED
					next_spline_camera++;
					cunt++;
				}

				current_spline_camera++;

				if(current_spline_camera > last_camera) {
					if(FirstCam->flags & 4) {
						spotcam_loopcnt++;
						current_spline_camera = first_camera;
					} else if(FirstCam->flags & 0x40 || spline_to_camera) {
						if(bCheckTrigger) {
							ctype = camera.type;
							camera.type = HEAVY_CAMERA;

							if(gfCurrentLevel) {
								TestTriggersAtXYZ(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, camera.pos.room_number, 1, 0);
							} else {
								TestTriggersAtXYZ(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, camera.pos.room_number, 0, 0);
								TestTriggersAtXYZ(camera.pos.pos.x, camera.pos.pos.y, camera.pos.pos.z, camera.pos.room_number, 1, 0);
							}

							camera.type = ctype;
							bCheckTrigger = 0;
						}

						SetFadeClip(0, 1);
						bUseSpotCam = 0;
						EnableLaraControl();
						bCheckTrigger = 0;
						camera.old_type = FIXED_CAMERA;
						camera.type = CHASE_CAMERA;
						camera.speed = 1;

						if(FirstCam->flags & 0x40) {
							camera.pos.pos.x = InitialCameraPosition.x;
							camera.pos.pos.y = InitialCameraPosition.y;
							camera.pos.pos.z = InitialCameraPosition.z;
							camera.target.pos.x = InitialCameraTarget.x;
							camera.target.pos.y = InitialCameraTarget.y;
							camera.target.pos.z = InitialCameraTarget.z;
							camera.pos.room_number = InitialCameraRoom;
						}

						AlterFOV(LastFov);
					} else {
						current_spline_camera--;
						SetSplineData(0, current_spline_camera - 1); // both inlined
						SetSplineData(1, current_spline_camera);
						memcpy(&backup, &camera, sizeof(CAMERA_INFO));
						camera.old_type = FIXED_CAMERA;
						camera.type = CHASE_CAMERA;
						camera.speed = 1;
						CalculateCamera();
						camera_fov[2] = CurrentFov;
						InitialCameraPosition.x = camera.pos.pos.x;
						InitialCameraPosition.y = camera.pos.pos.y;
						InitialCameraPosition.z = camera.pos.pos.z;
						InitialCameraTarget.x = camera.target.pos.x;
						InitialCameraTarget.y = camera.target.pos.y;
						InitialCameraTarget.z = camera.target.pos.z;
						camera_xposition[2] = camera.pos.pos.x;
						camera_yposition[2] = camera.pos.pos.y;
						camera_zposition[2] = camera.pos.pos.z;
						camera_xtarget[2] = camera.target.pos.x;
						camera_ytarget[2] = camera.target.pos.y;
						camera_ztarget[2] = camera.target.pos.z;
						camera_roll[2] = 0;
						camera_speed[2] = camera_speed[1];
						camera_xposition[3] = camera.pos.pos.x;
						camera_yposition[3] = camera.pos.pos.y;
						camera_zposition[3] = camera.pos.pos.z;
						camera_xtarget[3] = camera.target.pos.x;
						camera_ytarget[3] = camera.target.pos.y;
						camera_ztarget[3] = camera.target.pos.z;
						camera_fov[3] = CurrentFov;
						camera_speed[3] = camera_speed[1] >> 1;
						memcpy(&camera, &backup, sizeof(CAMERA_INFO));
						phd_LookAt(backup.pos.pos.x, backup.pos.pos.y, backup.pos.pos.z, backup.target.pos.x, backup.target.pos.y, backup.target.pos.z, (short)croll);
						spline_to_camera = 1;
					}
				}
			}
		}
	} else if(FirstCam->flags & 8) {
		if(!bFirstLook) {
			camera.old_type = FIXED_CAMERA;
			bFirstLook = 1;
		}

		CalculateCamera();
	} else {
		SetFadeClip(0, 1);
		bUseSpotCam = 0;
		EnableLaraControl();
		camera.speed = 1;
		AlterFOV(LastFov);
		CalculateCamera();
		bCheckTrigger = 0;
	}
}
