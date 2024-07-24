#ifndef TOMB4_SPECIFIC_INPUT_H
#define TOMB4_SPECIFIC_INPUT_H
#include "game/inputbuttons.h"
typedef enum keyboard_button {
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCTRL,
	KEY_RCTRL,
	KEY_SPACE,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_DEL,
	KEY_INSERT,
	KEY_POS1,
	KEY_END,
	KEY_PGUP,
	KEY_PGDWN,
	KEY_NUMPAD0,
	KEY_NUMPAD9 = KEY_NUMPAD0 + 9,
	KEY_NUMPADENTER,
	KEY_COMMA,
	KEY_DOT,
	KEY_DASH,
	KEY_RETURN,
	KEY_ESC,
	KEY_0,
	KEY_9 = KEY_0 + 9,
	KEY_A,
	KEY_Z = KEY_A + 25,
	KEY_F1,
	KEY_F12 = KEY_F1 + 11,
	NUM_KEYBOARD_BUTTONS,
} keyboard_button;

typedef enum gamepad_button {
	GAMEPAD_LX,
	GAMEPAD_LY,
	GAMEPAD_LZ,
	GAMEPAD_RX,
	GAMEPAD_RY,
	GAMEPAD_RZ,
	GAMEPAD_UP,
	GAMEPAD_DOWN,
	GAMEPAD_LEFT,
	GAMEPAD_RIGHT,
	GAMEPAD_LT,
	GAMEPAD_RT,
	GAMEPAD_LT2,
	GAMEPAD_RT2,
	GAMEPAD_A,
	GAMEPAD_B,
	GAMEPAD_C,
	GAMEPAD_D,
	GAMEPAD_E,
	GAMEPAD_F,
	GAMEPAD_START,
	GAMEPAD_SELECT,
	NUM_GAMEPAD_BUTTONS
} gamepad_button;

typedef struct INPUT_MANAGER INPUT_MANAGER;
long S_CreateInputManager(INPUT_MANAGER** out);
void S_DestroyInputManager(INPUT_MANAGER* manager);
void S_UpdateInput(INPUT_MANAGER* manager);
void S_ClearInput(INPUT_MANAGER* manager);
long S_IsActionDownDebounced(INPUT_MANAGER* manager, input_buttons button);
long S_IsActionDown(INPUT_MANAGER* manager, input_buttons button);
long S_MapKeyboardButton(INPUT_MANAGER* manager, keyboard_button button, input_buttons action);
long S_MapGamepadButton(INPUT_MANAGER* manager, gamepad_button button, input_buttons action);
long ReadJoystick(long* x, long* y);

extern const char* KeyboardButtons[272];
extern const char* GermanKeyboard[272];
extern short layout[2][18];

extern long conflict[18];
extern short ammo_change_timer;
extern char ammo_change_buf[12];

extern INPUT_MANAGER* inputImpl;
#endif // TOMB4_SPECIFIC_INPUT_H
