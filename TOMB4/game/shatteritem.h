#ifndef TOMB4_GAME_SHATTERITEM_H
#define TOMB4_GAME_SHATTERITEM_H
#include "sphere.h"

struct ITEM_LIGHT;
struct SHATTER_ITEM {
	SPHERE Sphere;
	ITEM_LIGHT* il;
	short* meshp;
	long Bit;
	short YRot;
	short Flags;
};
#endif // TOMB4_GAME_SHATTERITEM_H
