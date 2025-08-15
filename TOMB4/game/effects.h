#ifndef TOMB4_GAME_EFFECTS_H
#define TOMB4_GAME_EFFECTS_H
#include "game/phdvector.h"
typedef struct ITEM_INFO ITEM_INFO;
typedef struct FX_INFO FX_INFO;
typedef struct OBJECT_VECTOR OBJECT_VECTOR;
typedef struct PHD_3DPOS PHD_3DPOS;
typedef struct GAME_VECTOR GAME_VECTOR;
void WaterFall(short item_number);
void WadeSplash(ITEM_INFO* item, int water, int depth);
short DoBloodSplat(int x, int y, int z, short speed, short ang, PHD_VECTOR dir, short room_number);
void DoLotsOfBlood(int x, int y, int z, short speed, short ang, short room_number, int num);
void Richochet(GAME_VECTOR* pos);
void SoundEffects();
int ItemNearLara(PHD_3DPOS* pos, int rad);
void KillActiveBaddies(ITEM_INFO* item);
void Splash(ITEM_INFO* item);

extern int GlobalFogOff;
extern int FogTableColor[28];
extern void (*effect_routines[47])(ITEM_INFO* item);



#endif// TOMB4_GAME_EFFECTS_H
