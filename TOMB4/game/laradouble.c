
#include "game/laradouble.h"
#include "game/box.h"
#include "game/control.h"
#include "game/iteminfo.h"
#include "game/lara.h"
#include "game/levelinfo.h"
#include "game/sound.h"


void InitialiseLaraDouble(short item_number) {
	InitialiseCreature(item_number);
}

void LaraDoubleControl(short item_number) {
	ITEM_INFO* item;

	item = GetItem(currentLevel, item_number);
	SoundEffect(SFX_METAL_SCRAPE_LOOP1, (PHD_VECTOR*)&item->pos, item->room_number, SFX_DEFAULT);

	if(!CreatureActive(item_number)) {
		return;
	}

	if(item->hit_status) {
		lara_item->hit_points += item->hit_points - 1000;
	}

	item->hit_points = 1000;
	AnimateItem(item);
}
