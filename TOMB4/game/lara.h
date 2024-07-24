#ifndef TOMB4_GAME_LARA_H
#define TOMB4_GAME_LARA_H
#include "game/lara_states.h"
typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;
typedef struct LARA_INFO LARA_INFO;


extern void (*lara_control_routines[NUM_LARA_STATES])(ITEM_INFO* item, COLL_INFO* coll);
extern void (*lara_collision_routines[NUM_LARA_STATES])(ITEM_INFO* item, COLL_INFO* coll);

void LookUpDown();
void LookLeftRight();

extern LARA_INFO lara;
extern ITEM_INFO* lara_item;
extern short DashTimer;


#endif// TOMB4_GAME_LARA_H
