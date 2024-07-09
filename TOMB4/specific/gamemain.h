#pragma once
#include "game/watertab.h"
void S_GameClose();
long S_GameInitialise();
long S_SaveGame(long slot_num);
long S_LoadGame(long slot_num);
void S_GameMain();
extern WATERTAB WaterTable[22][64];
extern short* clipflags;
extern float vert_wibble_table[32];
extern long SaveCounter;
