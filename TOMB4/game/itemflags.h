#ifndef TOMB4_GAME_ITEMFLAGS_H
#define TOMB4_GAME_ITEMFLAGS_H
enum ITEM_FLAGS {
	IFL_TRIGGERED = 0x20,
	IFL_SWITCH_ONESHOT = 0x40, // oneshot for switch items
	IFL_ANTITRIGGER_ONESHOT = 0x80, // oneshot for antitriggers
	IFL_INVISIBLE = 0x100, // also used as oneshot for everything else
	IFL_CODEBITS = 0x3E00,
	IFL_REVERSE = 0x4000,
	IFL_CLEARBODY = 0x8000
};
#endif // TOMB4_GAME_ITEMFLAGS_H
