#ifndef TOMB4_GAME_LOTINFO_H
#define TOMB4_GAME_LOTINFO_H
#include "game/zonetype.h"
#include "game/phdvector.h"
typedef struct BOX_NODE BOX_NODE;
typedef struct LOT_INFO {
	BOX_NODE* node;
	short head;
	short tail;
	unsigned short search_number;
	unsigned short block_mask;
	short step;
	short drop;
	short zone_count;
	short target_box;
	short required_box;
	short fly;
	unsigned short can_jump : 1;
	unsigned short can_monkey : 1;
	unsigned short is_amphibious : 1;
	unsigned short is_jumping : 1;
	unsigned short is_monkeying : 1;
	PHD_VECTOR target;
	zone_type zone;
} LOT_INFO;
#endif // TOMB4_GAME_LOTINFO_H
