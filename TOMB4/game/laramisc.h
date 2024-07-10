#ifndef TOMB4_GAME_LARAMISC_H
#define TOMB4_GAME_LARAMISC_H
#include "game/collinfo.h"

typedef struct ITEM_INFO ITEM_INFO;


void LaraCheatyBits();
void InitialiseLaraLoad(short item_number);
void InitialiseLaraAnims(ITEM_INFO* item);
void LaraInitialiseMeshes();
void AnimateLara(ITEM_INFO* item);
void LaraControl(short item_number);

extern COLL_INFO mycoll;


#endif// TOMB4_GAME_LARAMISC_H
