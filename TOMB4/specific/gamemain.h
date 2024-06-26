#pragma once
#include "game/watertab.h"
#include "specific/thread.h"
#include <stdbool.h>
#include <Windows.h>
void GameClose();
unsigned int WINAPI GameMain(void* ptr);
unsigned short GetRandom(WATERTAB* wt, long lp);
void init_water_table();
bool GameInitialise();
long S_SaveGame(long slot_num);
long S_LoadGame(long slot_num);

extern WATERTAB WaterTable[22][64];
extern THREAD MainThread;
extern short* clipflags;
extern float vert_wibble_table[32];
extern long SaveCounter;
