#ifndef TOMB4_GAME_ATTACHEDITEMINFO_H
#define TOMB4_GAME_ATTACHEDITEMINFO_H
#include "game/objects.h"
#include "lara.h"
typedef struct ATTACHEDITEM_INFO {
	short item_carrier;
	short node;
	enum object_types object;
} ATTACHEDITEM_INFO;


void AttachedItemControl(short item_num);
void DrawAttachedItem(ITEM_INFO* item);

#endif// TOMB4_GAME_ATTACHEDITEMINFO_H
