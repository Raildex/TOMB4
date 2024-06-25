#pragma once
#include "game/collinfo.h"

struct ITEM_INFO;


void LaraCheatyBits();
void InitialiseLaraLoad(short item_number);
void InitialiseLaraAnims(ITEM_INFO* item);
void LaraInitialiseMeshes();
void AnimateLara(ITEM_INFO* item);
void LaraControl(short item_number);

extern COLL_INFO mycoll;
