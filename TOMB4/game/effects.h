#pragma once
typedef struct ITEM_INFO ITEM_INFO;
typedef struct FX_INFO FX_INFO;
typedef struct OBJECT_VECTOR OBJECT_VECTOR;
typedef struct PHD_3DPOS PHD_3DPOS;
typedef struct GAME_VECTOR GAME_VECTOR;
void WaterFall(short item_number);
void WadeSplash(ITEM_INFO* item, long water, long depth);
short DoBloodSplat(long x, long y, long z, short speed, short ang, short room_number);
void DoLotsOfBlood(long x, long y, long z, short speed, short ang, short room_number, long num);
void Richochet(GAME_VECTOR* pos);
void SoundEffects();
long ItemNearLara(PHD_3DPOS* pos, long rad);
void KillActiveBaddies(ITEM_INFO* item);
void Splash(ITEM_INFO* item);

extern long GlobalFogOff;
extern long FogTableColor[28];
extern void (*effect_routines[47])(ITEM_INFO* item);

