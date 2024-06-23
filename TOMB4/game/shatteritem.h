#ifndef SHATTER_ITEM_INCLUDED
#define SHATTER_ITEM_INCLUDED
#include "sphere.h"

struct ITEM_LIGHT;
struct SHATTER_ITEM
{
	SPHERE Sphere;
	ITEM_LIGHT* il;
	short* meshp;
	long Bit;
	short YRot;
	short Flags;
};
#endif