
#include "game/clockworkbeetle.h"
#include "game/animstruct.h"
#include "game/control.h"
#include "game/delstuff.h"
#include "game/heighttypes.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/itemstatus.h"
#include "game/lara.h"
#include "game/lara_states.h"
#include "game/laragunstatus.h"
#include "game/larainfo.h"
#include "game/levelinfo.h"
#include "game/objects.h"
#include "game/phdvector.h"
#include "game/roominfo.h"
#include "game/sound.h"
#include "global/types.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include <stdlib.h>

ITEM_INFO* TriggerClockworkBeetle(long flag) {
	ITEM_INFO* item;
	ITEM_INFO* item2;
	long dx, dy, dz;
	short item_number;

	if(flag || (lara_item->current_anim_state == AS_STOP && lara_item->anim_number == ANIM_BREATH && !lara_item->gravity_status && lara.gun_status == LG_NO_ARMS)) {
		item_number = CreateItem();

		if(item_number != NO_ITEM) {
			lara.mechanical_scarab &= ~1;
			item = GetItem(currentLevel, item_number);
			item->shade = -0x3DF0;
			item->object_number = CLOCKWORK_BEETLE;
			item->room_number = lara_item->room_number;
			item->pos.pos.x = lara_item->pos.pos.x;
			item->pos.pos.y = lara_item->pos.pos.y;
			item->pos.pos.z = lara_item->pos.pos.z;
			InitialiseItem(item_number);
			item->pos.z_rot = 0;
			item->pos.x_rot = 0;
			item->pos.y_rot = lara_item->pos.y_rot;

			if(lara.beetle_uses) {
				item->item_flags[0] = lara.OnBeetleFloor;
			} else {
				item->item_flags[0] = 0;
			}

			item->speed = 0;
			AddActiveItem(item_number);

			if(item->item_flags[0]) {
				for(item_number = GetRoom(currentLevel, item->room_number)->item_number; item_number != NO_ITEM; item_number = item2->next_item) {
					item2 = GetItem(currentLevel, item_number);

					if(item2->object_number == MAPPER) {
						dx = item->pos.pos.x - item2->pos.pos.x;
						dy = item->pos.pos.y - item2->pos.pos.y;
						dz = item->pos.pos.z - item2->pos.pos.z;

						if(dx > -1024 && dx < 1024 && dz > -1024 && dz < 1024 && dy > -1024 && dy < 1024) {
							item->item_flags[1] = item2->pos.y_rot + 0x8000;

							if(item2->item_flags[0]) {
								item->item_flags[0] = 0;
							} else {
								item2->item_flags[0] = 1;
							}
						}
					}
				}
			}

			if(!item->item_flags[0]) {
				item->item_flags[3] = 150;
			}

			return item;
		}
	}

	return 0;
}

void ControlClockworkBeetle(short item_number) {
	ITEM_INFO* item;
	ITEM_INFO* item2;
	FLOOR_INFO* floor;
	height_types ht;
	long tiltxoff, tiltzoff, OnObject;
	PHD_VECTOR pos;
	long bounce, h, x, z, dx, dy, dz;
	short frame, base, room_number, rotY, angle;

	bounce = 0;
	item = GetItem(currentLevel, item_number);

	if(lara_item->anim_number == ANIM_USEBEETLE) {
		frame = lara_item->frame_number;
		base = GetAnim(currentLevel, ANIM_USEBEETLE)->frame_base;

		if(frame < base + 14) {
			item->status = ITEM_INVISIBLE;
			return;
		}

		if(frame < base + 104) {
			pos.x = 0;
			pos.y = 0;
			pos.z = -32;
			GetLaraJointPos(&pos, 11);
			item->pos.pos.x = pos.x;
			item->pos.pos.y = pos.y;
			item->pos.pos.z = pos.z;
			item->pos.y_rot = lara_item->pos.y_rot;
			item->pos.z_rot = -12740;
			item->status = ITEM_ACTIVE;
			return;
		}

		if(frame == base + 104) {
			room_number = item->room_number;
			floor = GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);
			h = GetHeight(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &ht, &tiltxoff, &tiltzoff, &OnObject);

			if(abs(lara_item->pos.pos.y - h) > 64) {
				item->pos.pos.x = lara_item->pos.pos.x;
				item->pos.pos.y = lara_item->pos.pos.y;
				item->pos.pos.z = lara_item->pos.pos.z;
			}

			return;
		}
	}

	SoundEffect(SFX_BEETLE_CLK_WHIRR, (PHD_VECTOR*)&item->pos, SFX_DEFAULT);
	item->fallspeed += 12;
	item->pos.pos.y += item->fallspeed;
	room_number = item->room_number;
	floor = GetFloor(item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &room_number);
	h = GetHeight(floor, item->pos.pos.x, item->pos.pos.y, item->pos.pos.z, &ht, &tiltxoff, &tiltzoff, &OnObject);

	if(item->pos.pos.y > h) {
		item->pos.pos.y = h;

		if(item->fallspeed <= 32) {
			item->fallspeed = 0;
		} else {
			item->fallspeed = -item->fallspeed >> 1;
		}

		bounce = 1;
	}

	TestTriggers(trigger_index, 0, 0);

	if(room_number != item->room_number) {
		ItemNewRoom(item_number, room_number);
	}

	if(item->item_flags[0]) {
		item->pos.z_rot = (0x1000 * phd_sin(0x1000 * (GlobalCounter & 15))) >> W2V_SHIFT;

		switch(item->item_flags[2]) {
		case 0:
			x = ((item->pos.pos.x & -512) | 512) - item->pos.pos.x;
			z = ((item->pos.pos.z & -512) | 512) - item->pos.pos.z;

			if(x > -8 && z > -8 && x < 8 && z < 8) {
				item->pos.pos.x &= -512;
				item->pos.pos.z &= -512;
				item->item_flags[2] = 2;
			} else {
				angle = (short)phd_atan(z, x);
				rotY = angle - item->pos.y_rot;

				if(abs(rotY) > 0x8000) {
					rotY = item->pos.y_rot - angle;
				}

				if(abs(rotY) < 256) {
					item->pos.y_rot = angle;
					item->item_flags[2] = 1;
				} else if(rotY < 0) {
					item->pos.y_rot -= 256;
				} else {
					item->pos.y_rot += 256;
				}
			}

			break;

		case 1:
		case 4:
			x = ((item->pos.pos.x & -512) | 512) - item->pos.pos.x;
			z = ((item->pos.pos.z & -512) | 512) - item->pos.pos.z;

			if(x > -8 && z > -8 && x < 8 && z < 8) {
				item->pos.pos.x = (item->pos.pos.x & -512) | 512;
				item->pos.pos.z = (item->pos.pos.z & -512) | 512;

				if(item->item_flags[2] == 1) {
					item->item_flags[2] = 2;
				} else {
					lara.beetle_uses--;
					item->item_flags[2] = 5;

					for(int i = GetRoom(currentLevel, item->room_number)->item_number; i != NO_ITEM; i = item2->next_item) {
						item2 = GetItem(currentLevel, i);

						if(item2->object_number == MAPPER) {
							dx = item->pos.pos.x - item2->pos.pos.x;
							dy = item->pos.pos.y - item2->pos.pos.y;
							dz = item->pos.pos.z - item2->pos.pos.z;

							if(dx > -1024 && dx < 1024 && dz > -1024 && dz < 1024 && dy > -1024 && dy < 1024) {
								item2->item_flags[0] = 1;
							}
						}
					}
				}
			} else {
				angle = (short)phd_atan(z, x);
				item->pos.y_rot = angle;

				if(SQUARE(x) + SQUARE(z) >= 0x19000) {
					if(item->speed < 32) {
						item->speed++;
					}
				} else if(item->speed > 4) {
					if(item->item_flags[2] == 4) {
						item->speed -= 2;
					} else {
						item->speed--;
					}
				} else if(item->speed < 4) {
					item->speed++;
				}

				item->pos.pos.x += item->speed * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
				item->pos.pos.z += item->speed * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
			}

			break;

		case 2:
			rotY = item->item_flags[1] - item->pos.y_rot;

			if(abs(rotY) > 0x8000) {
				rotY = item->pos.y_rot - item->item_flags[1];
			}

			if(abs(rotY) < 256) {
				item->item_flags[2] = 3;
				item->pos.y_rot = item->item_flags[1];
			} else if(rotY < 0) {
				item->pos.y_rot -= 256;
			} else {
				item->pos.y_rot += 256;
			}

			break;

		case 3:

			if(item->speed < 32) {
				item->speed++;
			}

			item->pos.pos.x += item->speed * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
			item->pos.pos.z += item->speed * phd_cos(item->pos.y_rot) >> W2V_SHIFT;

			if(!lara.OnBeetleFloor) {
				item->item_flags[3] = 1;
			} else if(item->item_flags[3]) {
				item->item_flags[2] = 4;
			}

			break;
		}
	} else {
		item->pos.z_rot = (0x2000 * phd_sin(0x2000 * (GlobalCounter & 7))) >> W2V_SHIFT;

		if(item->item_flags[3]) {
			item->item_flags[3]--;
		}

		if(lara.beetle_uses) {
			if(item->item_flags[3] > 75) {
				rotY = 150 - item->item_flags[3];
			} else {
				rotY = item->item_flags[3];
			}

			item->pos.y_rot += rotY << 5;
			rotY >>= 1;

			if(bounce && item->item_flags[3] > 30 && rotY) {
				item->fallspeed = -((rotY >> 1) + GetRandomControl() % rotY);
			}
		} else {
			item->pos.z_rot <<= 1;
			rotY = (150 - item->item_flags[3]) >> 1;
			item->pos.y_rot += rotY << 7;

			if(bounce && rotY) {
				item->fallspeed = -((rotY >> 1) + GetRandomControl() % rotY);
			} else if(item->item_flags[3] < 30) {
				SoundEffect(SFX_BEETLE_CLK_EXP, (PHD_VECTOR*)&item->pos, 0);
				ExplodeItemNode(item, 0, 0, 128);
				KillItem(item_number);
			}
		}
	}
}
