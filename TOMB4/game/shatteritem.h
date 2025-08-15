#ifndef TOMB4_GAME_SHATTERITEM_H
#define TOMB4_GAME_SHATTERITEM_H
#include "game/sphere.h"

typedef struct ITEM_LIGHT ITEM_LIGHT;
typedef struct SHATTER_ITEM {
	SPHERE Sphere;
	ITEM_LIGHT* il;
	short* meshp;
	int Bit;
	short YRot;
	short Flags;
} SHATTER_ITEM;
#endif// TOMB4_GAME_SHATTERITEM_H
