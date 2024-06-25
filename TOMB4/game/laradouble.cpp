
#include "laradouble.h"
#include "box.h"
#include "levelinfo.h"
#include "sound.h"
#include "control.h"
#include "lara.h"
#include "iteminfo.h"

void InitialiseLaraDouble(short item_number) {
	InitialiseCreature(item_number);
}

void LaraDoubleControl(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);
	SoundEffect(SFX_METAL_SCRAPE_LOOP1, &item->pos, SFX_DEFAULT);

	if(!CreatureActive(item_number))
		return;

	if(item->hit_status)
		lara_item->hit_points += item->hit_points - 1000;

	item->hit_points = 1000;
	AnimateItem(item);
}
