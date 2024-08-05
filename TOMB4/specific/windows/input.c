
#include "specific/input.h"
#include "game/camera.h"
#include "game/carriedweaponflags.h"
#include "game/control.h"
#include "game/gameflow.h"
#include "game/gfleveloptions.h"
#include "game/inputbuttons.h"
#include "game/iteminfo.h"
#include "game/lara.h"
#include "game/lara_states.h"
#include "game/laragunstatus.h"
#include "game/larainfo.h"
#include "game/larawaterstatus.h"
#include "game/newinv.h"
#include "game/savegame.h"
#include "game/savegameinfo.h"
#include "game/sound.h"
#include "game/spotcam.h"
#include "game/weapontypes.h"
#include "specific/function_stubs.h"
#include "specific/loadsave.h"
#include "specific/windows/dxshell.h"
#include "specific/windows/winmain.h"
#include <stdlib.h>
#include <string.h>
#include <dinput.h>
#include <joystickapi.h>
#include <libloaderapi.h>
#include <minwindef.h>
#include <stdio.h>
#include <unknwnbase.h>
#include <winbase.h>
#include <winerror.h>
#include <winnt.h>


typedef struct INPUT_MANAGER {
	IDirectInput8A* directInput;
	IDirectInputDevice8A* keyboard;
	IDirectInputDevice8A* gamepad;
	short lastInput[NUM_INPUTACTIONS]; // last frame's activated actions, Bit 15 = debounced
	input_buttons keyboardLayout[NUM_KEYBOARD_BUTTONS]; // internal button -> action
	input_buttons gamepadLayout[NUM_GAMEPAD_BUTTONS]; // internal gamepad button -> action
} INPUT_MANAGER;

const char* KeyboardButtons[272] = {
	0,
	"Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "Bksp",
	"Tab", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "<", ">", "Return",
	"Ctrl", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "`",
	"Shift", "#", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "Shift",
	"Padx", "Alt", "Space", "Caps", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Nmlk", 0,
	"Pad7", "Pad8", "Pad9", "Pad-",
	"Pad4", "Pad5", "Pad6", "Pad+",
	"Pad1", "Pad2", "Pad3",
	"Pad0", "Pad.", 0, 0, "\\", 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Enter", "Ctrl", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Shift", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Pad/", 0, 0, "Alt", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Home", "Up", "PgUp", 0, "Left", 0, "Right", 0, "End", "Down", "PgDn", "Ins", "Del",
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Joy 1", "Joy 2", "Joy 3", "Joy 4", "Joy 5", "Joy 6", "Joy 7", "Joy 8",
	"Joy 9", "Joy 10", "Joy 11", "Joy 12", "Joy 13", "Joy 14", "Joy 15", "Joy 16"
};

const char* GermanKeyboard[272] = {
	0,
	"Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "=", "`", "Entf",
	"Tab", "Q", "W", "E", "R", "T", "Z", "U", "I", "O", "P", "~u", "x", "Enter",
	"Strg", "A", "S", "D", "F", "G", "H", "J", "K", "L", "~o", "~a", "( ",
	"Shift", "#", "Y", "X", "C", "V", "B", "N", "M", ",", ".", "-", "Shift",
	"Num x", "Alt", "Leert.", "Caps", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Nmlk", 0,
	"Num7", "Num8", "Num9", "Num-",
	"Num4", "Num5", "Num6", "Num+",
	"Num1", "Num2", "Num3",
	"Num0", "Num.", 0, 0, ">", 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Enter", "Strg", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Shift", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Num/", 0, 0, "Alt", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Pos1", "Hoch", "BdAuf", 0, "Links", 0, "Rechts", 0, "Ende", "Runter", "BdAb", "Einfg", "Entf",
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Joy 1", "Joy 2", "Joy 3", "Joy 4", "Joy 5", "Joy 6", "Joy 7", "Joy 8",
	"Joy 9", "Joy 10", "Joy 11", "Joy 12", "Joy 13", "Joy 14", "Joy 15", "Joy 16"
};

short layout[2][18] = {
	{ DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_PERIOD, DIK_SLASH, DIK_RSHIFT, DIK_RALT, DIK_RCONTROL,
		DIK_SPACE, DIK_COMMA, DIK_NUMPAD0, DIK_END, DIK_ESCAPE, DIK_DELETE, DIK_PGDN, DIK_P, DIK_RETURN },

	{ DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_PERIOD, DIK_SLASH, DIK_RSHIFT, DIK_RALT, DIK_RCONTROL,
		DIK_SPACE, DIK_COMMA, DIK_NUMPAD0, DIK_END, DIK_ESCAPE, DIK_DELETE, DIK_PGDN, DIK_P, DIK_RETURN }
};

const int DirectInputKeyMap[256] = { // DIK -> Internal
	-1,
	KEY_ESC,
	KEY_0 + 1,
	KEY_0 + 2,
	KEY_0 + 3,
	KEY_0 + 4,
	KEY_0 + 5,
	KEY_0 + 6,
	KEY_0 + 7,
	KEY_0 + 8,
	KEY_0 + 9,
	KEY_0,
	KEY_DASH,
	-1,
	-1,
	-1,
	KEY_A + ('Q' - 'A'),
	KEY_A + ('W' - 'A'),
	KEY_A + ('E' - 'A'),
	KEY_A + ('R' - 'A'),
	KEY_A + ('T' - 'A'),
	KEY_A + ('Y' - 'A'),
	KEY_A + ('U' - 'A'),
	KEY_A + ('I' - 'A'),
	KEY_A + ('O' - 'A'),
	KEY_A + ('P' - 'A'),
	-1,
	-1,
	KEY_RETURN,
	KEY_LCTRL,
	KEY_A,
	KEY_A + ('S' - 'A'),
	KEY_A + ('D' - 'A'),
	KEY_A + ('F' - 'A'),
	KEY_A + ('G' - 'A'),
	KEY_A + ('H' - 'A'),
	KEY_A + ('J' - 'A'),
	KEY_A + ('K' - 'A'),
	KEY_A + ('L' - 'A'),
	-1,
	-1,
	-1,
	KEY_LSHIFT,
	-1,
	KEY_A + ('Z' - 'A'),
	KEY_A + ('X' - 'A'),
	KEY_A + ('C' - 'A'),
	KEY_A + ('V' - 'A'),
	KEY_A + ('B' - 'A'),
	KEY_A + ('N' - 'A'),
	KEY_A + ('M' - 'A'),
	KEY_COMMA,
	KEY_DOT,
	KEY_DASH,
	KEY_RSHIFT,
	-1,
	KEY_LALT,
	KEY_SPACE,
	-1,
	KEY_F1,
	KEY_F1 + 1,
	KEY_F1 + 2,
	KEY_F1 + 3,
	KEY_F1 + 4,
	KEY_F1 + 5,
	KEY_F1 + 6,
	KEY_F1 + 7,
	KEY_F1 + 8,
	KEY_F1 + 9,
	-1,
	-1,
	KEY_NUMPAD0 + 7,
	KEY_NUMPAD0 + 8,
	KEY_NUMPAD0 + 9,
	-1,
	KEY_NUMPAD0 + 4,
	KEY_NUMPAD0 + 5,
	KEY_NUMPAD0 + 6,
	-1,
	KEY_NUMPAD0 + 1,
	KEY_NUMPAD0 + 2,
	KEY_NUMPAD0 + 3,
	KEY_NUMPAD0,
	-1,
	-1,
	-1,
	-1,
	KEY_F1 + 10,
	KEY_F1 + 11,
	-1, // DIK_KANA
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	KEY_LCTRL,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	KEY_LALT,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	KEY_POS1,
	KEY_UP,
	KEY_PGUP,
	-1,
	KEY_LEFT,
	-1,
	KEY_RIGHT,
	-1,
	KEY_END,
	KEY_DOWN,
	KEY_PGDWN,
	KEY_INSERT,
	KEY_DEL,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
};

long conflict[18];
short ammo_change_timer = 0;
char ammo_change_buf[12];

static long joy_x;
static long joy_y;
static long joy_fire;

static void DoWeaponHotkey() // adds extra checks and does ammo type swaps..
{
	short state;
	long goin;

	if(!lara_item) {
		goin = 0;
	} else {
		state = lara_item->current_anim_state;
		goin = !(gfLevelFlags & GF_YOUNGLARA) && (lara.water_status == LW_ABOVE_WATER || lara.water_status == LW_WADE) && !bDisableLaraControl && (state != AS_ALL4S && state != AS_CRAWL && state != AS_ALL4TURNL && state != AS_ALL4TURNR && state != AS_CRAWLBACK && state != AS_CRAWL2HANG && state != AS_DUCK && state != AS_DUCKROTL && state != AS_DUCKROTR);
	}

	if(!goin) {
		return;
	}

	if(keymap[DIK_1]) {
		if(!(lara.pistols_type_carried & W_PRESENT)) {
			return;
		}

		lara.request_gun_type = WEAPON_PISTOLS;

		if(lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_PISTOLS) {
			lara.gun_status = LG_DRAW_GUNS;
		}
	} else if(keymap[DIK_2]) {
		if(!(lara.shotgun_type_carried & W_PRESENT)) {
			return;
		}

		lara.request_gun_type = WEAPON_SHOTGUN;

		if(lara.gun_type == WEAPON_SHOTGUN) {
			if(lara.gun_status == LG_NO_ARMS) {
				lara.gun_status = LG_DRAW_GUNS;
			} else if(lara.gun_status == LG_READY && !ammo_change_timer) {
				if(1) {
					return;
				}

				memset(ammo_change_buf, 0, sizeof(ammo_change_buf));

				if(lara.shotgun_type_carried & W_AMMO2) {
					lara.shotgun_type_carried &= ~W_AMMO2;
					lara.shotgun_type_carried |= W_AMMO1;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Normal");
				} else if(lara.shotgun_type_carried & W_AMMO1) {
					lara.shotgun_type_carried &= ~W_AMMO1;
					lara.shotgun_type_carried |= W_AMMO2;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Wideshot");
				}
			}
		}
	} else if(keymap[DIK_3]) {
		if(!(lara.uzis_type_carried & W_PRESENT)) {
			return;
		}

		lara.request_gun_type = WEAPON_UZI;

		if(lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_UZI) {
			lara.gun_status = LG_DRAW_GUNS;
		}
	} else if(keymap[DIK_4]) {
		if(!(lara.sixshooter_type_carried & W_PRESENT)) {
			return;
		}

		lara.request_gun_type = WEAPON_REVOLVER;

		if(lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_REVOLVER) {
			lara.gun_status = LG_DRAW_GUNS;
		}
	} else if(keymap[DIK_5]) {
		if(!(lara.grenade_type_carried & W_PRESENT)) {
			return;
		}

		lara.request_gun_type = WEAPON_GRENADE;

		if(lara.gun_type == WEAPON_GRENADE) {
			if(lara.gun_status == LG_NO_ARMS) {
				lara.gun_status = LG_DRAW_GUNS;
			} else if(lara.gun_status == LG_READY && !ammo_change_timer) {
				if(1) {
					return;
				}

				memset(ammo_change_buf, 0, sizeof(ammo_change_buf));

				if(lara.grenade_type_carried & W_AMMO3) {
					lara.grenade_type_carried &= ~W_AMMO3;
					lara.grenade_type_carried |= W_AMMO2;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Super");
				} else if(lara.grenade_type_carried & W_AMMO2) {
					lara.grenade_type_carried &= ~W_AMMO2;
					lara.grenade_type_carried |= W_AMMO1;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Normal");
				} else if(lara.grenade_type_carried & W_AMMO1) {
					lara.grenade_type_carried &= ~W_AMMO1;
					lara.grenade_type_carried |= W_AMMO3;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Flash");
				}
			}
		}
	} else if(keymap[DIK_6]) {
		if(!(lara.crossbow_type_carried & W_PRESENT)) {
			return;
		}

		lara.request_gun_type = WEAPON_CROSSBOW;

		if(lara.gun_type == WEAPON_CROSSBOW) {
			if(lara.gun_status == LG_NO_ARMS) {
				lara.gun_status = LG_DRAW_GUNS;
			} else if(lara.gun_status == LG_READY && !ammo_change_timer) {
				if(1) {
					return;
				}

				memset(ammo_change_buf, 0, sizeof(ammo_change_buf));

				if(lara.crossbow_type_carried & W_AMMO3) {
					lara.crossbow_type_carried &= ~W_AMMO3;
					lara.crossbow_type_carried |= W_AMMO2;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Poison");
				} else if(lara.crossbow_type_carried & W_AMMO2) {
					lara.crossbow_type_carried &= ~W_AMMO2;
					lara.crossbow_type_carried |= W_AMMO1;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Normal");
				} else if(lara.crossbow_type_carried & W_AMMO1) {
					lara.crossbow_type_carried &= ~W_AMMO1;
					lara.crossbow_type_carried |= W_AMMO3;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Explosive");
				}
			}
		}
	}
}

long Key(long number) {
	short key;

	key = layout[1][number];

	if(key < 256) {
		if(keymap[key]) {
			return 1;
		}

		switch(key) {
		case DIK_RCONTROL:
			return keymap[DIK_LCONTROL];

		case DIK_LCONTROL:
			return keymap[DIK_RCONTROL];

		case DIK_RSHIFT:
			return keymap[DIK_LSHIFT];

		case DIK_LSHIFT:
			return keymap[DIK_RSHIFT];

		case DIK_RMENU:
			return keymap[DIK_LMENU];

		case DIK_LMENU:
			return keymap[DIK_RMENU];
		}
	} else if(joy_fire & (1 << key)) {
		return 1;
	}

	if(conflict[number]) {
		return 0;
	}

	key = layout[0][number];

	if(keymap[key]) {
		return 1;
	}

	switch(key) {
	case DIK_RCONTROL:
		return keymap[DIK_LCONTROL];

	case DIK_LCONTROL:
		return keymap[DIK_RCONTROL];

	case DIK_RSHIFT:
		return keymap[DIK_LSHIFT];

	case DIK_LSHIFT:
		return keymap[DIK_RSHIFT];

	case DIK_RMENU:
		return keymap[DIK_LMENU];

	case DIK_LMENU:
		return keymap[DIK_RMENU];
	}

	return 0;
}

long S_IsActionDown(INPUT_MANAGER* manager, input_buttons button) {
	return manager->lastInput[button] & 0x0001;
}

long S_IsActionDownDebounced(INPUT_MANAGER* manager, input_buttons button) {
	return manager->lastInput[button] & 0x0001 && !(manager->lastInput[button] & 0x8000);
}

long S_MapKeyboardButton(INPUT_MANAGER *manager, keyboard_button button, input_buttons action) {
	for(int i = 0; i < NUM_KEYBOARD_BUTTONS; ++i) {
		if(manager->keyboardLayout[i] == action) {
			return 0; // we have a button set to an action already
		}
	}
	manager->keyboardLayout[button] = action;
	return 1;
}

void S_UpdateInput(INPUT_MANAGER* manager) {
	char keymap[NUM_KEYBOARD_BUTTONS] = {0};
	char directInputKeymap[256];
	short frameInput[NUM_INPUTACTIONS] = {0};
	HRESULT state = IDirectInputDevice8_GetDeviceState(manager->keyboard, 256, directInputKeymap);

	if(FAILED(state)) {
		if(state == DIERR_INPUTLOST) {
			IDirectInputDevice8_Acquire(manager->keyboard);
		}
		IDirectInputDevice8_GetDeviceState(manager->keyboard, 256, directInputKeymap);
	}
	for(int i = 0; i < 256; ++i) {
		if(DirectInputKeyMap[i] == -1) {
			continue;
		}
		long internalKey = DirectInputKeyMap[i];
		keymap[internalKey] = directInputKeymap[i];
		input_buttons action = manager->keyboardLayout[internalKey];
		if(action == IN_NONE) {
			continue;
		}
		if(keymap[internalKey]) {
			frameInput[action] = 1;
			if(manager->lastInput[action] & 0x1 || manager->lastInput[action] & 0x8000) {
				frameInput[action] |= (short)0x8000;
			}
		}
	}
	memcpy(&manager->lastInput[0], &frameInput[0], sizeof(frameInput));
}

void S_ClearInput(INPUT_MANAGER* manager) {
	memset(&manager->lastInput[0],0,sizeof(manager->lastInput));
}

long ReadJoystick(long* x, long* y) {
	JOYINFOEX joystick;
	static JOYCAPS caps;
	static long unavailable = 1;

	joystick.dwSize = sizeof(JOYINFOEX);
	joystick.dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS;

	if(joyGetPosEx(0, &joystick) != JOYERR_NOERROR) {
		unavailable = 1;
		x = 0;
		y = 0;
		return 0;
	}

	if(unavailable) {
		if(joyGetDevCaps(JOYSTICKID1, &caps, sizeof(caps)) != JOYERR_NOERROR) {
			x = 0;
			y = 0;
			return 0;
		} else {
			unavailable = 0;
		}
	}

	*x = (joystick.dwXpos << 5) / (caps.wXmax - caps.wXmin) - 16;
	*y = (joystick.dwYpos << 5) / (caps.wYmax - caps.wYmin) - 16;
	return joystick.dwButtons;
}

static long Check(const char* scope, HRESULT result) {
	if(!SUCCEEDED(result)) {
		char buffer[256];
		long n = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), buffer, sizeof(buffer), NULL);
		LogE(scope, "DirectInput Error: %.*s", n, buffer );
		__debugbreak();
		return 0;
	}
	return 1;
}

BOOL WINAPI EnumGamePad(const DIDEVICEINSTANCEA* instance, void* payload) {
	Log(__func__, "Found Device: %s",instance->tszInstanceName);
	INPUT_MANAGER* manager = (INPUT_MANAGER*)payload;
	if(!Check(__func__, IDirectInput8_CreateDevice(manager->directInput, &instance->guidInstance, &manager->gamepad, NULL))) {
		return DIENUM_CONTINUE;
	}
	if(!Check(__func__, IDirectInputDevice8_SetCooperativeLevel(manager->gamepad, App.hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))) {
		IUnknown_Release(manager->gamepad);
		return DIENUM_CONTINUE;
	}
	if(!Check(__func__, IDirectInputDevice8_Acquire(manager->gamepad))) {
		IUnknown_Release(manager->gamepad);
		return DIENUM_CONTINUE;
	}
	Log(__func__, "Acquired Device: %s", instance->tszInstanceName);
	return DIENUM_STOP;
}

static const input_buttons defaultLayout[NUM_KEYBOARD_BUTTONS] = {
	IN_FORWARD,
	IN_BACK,
	IN_LEFT,
	IN_RIGHT,
	IN_JUMP,
	IN_NONE,
	IN_ACTION,
	IN_NONE,
	IN_DRAW,
	IN_WALK,
	IN_NONE,
	IN_LSTEP,
	IN_NONE,
	IN_NONE,
	IN_ROLL,
	IN_NONE,
	IN_RSTEP,
	IN_LOOK,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_FLARE,
	IN_DUCK,
	IN_SPRINT,
	IN_SELECT,
	IN_DESELECT,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_CHEAT,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE,
	IN_NONE
};

void S_ClearAction(INPUT_MANAGER *manager, input_buttons button) {
	manager->lastInput[button] = 0;
}

long S_CreateInputManager(INPUT_MANAGER * *out) {
	INPUT_MANAGER* manager = (INPUT_MANAGER*)calloc(1, sizeof(INPUT_MANAGER));
	memcpy(manager->keyboardLayout, defaultLayout, sizeof(defaultLayout));
	if(!manager) {
		return 0;
	}
	HINSTANCE hinst = GetModuleHandle(NULL);
	if(!Check(__func__, DirectInput8Create(hinst, DIRECTINPUT_VERSION, &IID_IDirectInput8A, (void**)&manager->directInput, NULL))) {
		free(manager);
		return 0;
	}

	if(!Check(__func__, IDirectInput8_CreateDevice(manager->directInput, &GUID_SysKeyboard, &manager->keyboard, NULL))) {
		IUnknown_Release(manager->directInput);
		free(manager);
		return 0;
	}

	if(!Check(__func__, IDirectInputDevice8_SetDataFormat(manager->keyboard,&c_dfDIKeyboard))) {
		IUnknown_Release(manager->directInput);
		free(manager);
		return 0;
	}

	if(!Check(__func__, IDirectInputDevice8_SetCooperativeLevel(manager->keyboard, App.hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))) {
		IUnknown_Release(manager->keyboard);
		IUnknown_Release(manager->directInput);
		free(manager);
		return 0;
	}

	if(!Check(__func__, IDirectInputDevice8_Acquire(manager->keyboard))) {
		IUnknown_Release(manager->keyboard);
		IUnknown_Release(manager->directInput);
		free(manager);
		return 0;
	}

	if(!Check(__func__, IDirectInput8_EnumDevices(manager->directInput, DI8DEVCLASS_GAMECTRL, EnumGamePad, manager, DIEDFL_ATTACHEDONLY))) {
		IUnknown_Release(manager->keyboard);
		IUnknown_Release(manager->directInput);
		free(manager);
		return 0;
	}
	*out = manager;
	return 1;
}

void S_DestroyInputManager(INPUT_MANAGER *manager) {
	IUnknown_Release(manager->keyboard);
	if(manager->gamepad) {
		IUnknown_Release(manager->gamepad);
	}
	IUnknown_Release(manager->directInput);
	free(manager);
}

INPUT_MANAGER* inputImpl;