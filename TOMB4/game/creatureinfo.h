#ifndef CREATURE_INFO_INCLUDED
#define CREATURE_INFO_INCLUDED
#include "game/moodtype.h"
#include "game/lotinfo.h"
#include "game/phdvector.h"
#include "game/iteminfo.h"
struct CREATURE_INFO {
	short joint_rotation[4];
	short maximum_turn;
	short flags;
	unsigned short alerted : 1;
	unsigned short head_left : 1;
	unsigned short head_right : 1;
	unsigned short reached_goal : 1;
	unsigned short hurt_by_lara : 1;
	unsigned short patrol2 : 1;
	unsigned short jump_ahead : 1;
	unsigned short monkey_ahead : 1;
	mood_type mood;
	ITEM_INFO* enemy;
	ITEM_INFO ai_target;
	short pad;
	short item_num;
	PHD_VECTOR target;
	LOT_INFO LOT;
};
#endif