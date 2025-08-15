#ifndef TOMB4_GAME_EFFECT2_H
#define TOMB4_GAME_EFFECT2_H
#include "game/dynamic.h"
#include "game/splashstruct.h"
#include "game/ripplestruct.h"
#include "game/sparks.h"
typedef struct SPLASH_SETUP SPLASH_SETUP;
typedef struct GAME_VECTOR GAME_VECTOR;
typedef struct ITEM_INFO ITEM_INFO;
void ControlSmokeEmitter(short item_number);
void TriggerExplosionSmokeEnd(int x, int y, int z, int uw);
void TriggerExplosionSmoke(int x, int y, int z, int uw);
void TriggerFlareSparks(int x, int y, int z, int xvel, int yvel, int zvel, int smoke);
void TriggerDynamic(int x, int y, int z, int falloff, int r, int g, int b);
void TriggerDynamic_MIRROR(int x, int y, int z, int falloff, int r, int g, int b);
void ClearDynamics();
void ControlEnemyMissile(short fx_number);
void SetupRipple(int x, int y, int z, int size, int flags);
void TriggerUnderwaterBlood(int x, int y, int z, int size);
void TriggerWaterfallMist(int x, int y, int z, int ang);
void TriggerDartSmoke(int x, int y, int z, int xv, int zv, int hit);
void KillAllCurrentItems(short item_number);
void KillEverything();
void TriggerExplosionBubble(int x, int y, int z, short room_number);
void ControlColouredLights(short item_number);
void DetatchSpark(int num, int type);
SPARKS* GetFreeSpark();
void UpdateSparks();
void TriggerRicochetSpark(GAME_VECTOR* pos, int ang, int num, int smoke_only);
void TriggerExplosionSparks(int x, int y, int z, int extras, int dynamic, int uw, short room_number);
void TriggerFireFlame(int x, int y, int z, int body_part, int type);
void TriggerSuperJetFlame(ITEM_INFO* item, int yvel, int deadly);
void TriggerRocketSmoke(int x, int y, int z, int col);
void SetupSplash(SPLASH_SETUP* setup);
void UpdateSplashes();

extern DYNAMIC dynamics[];
extern int nSplashes;
extern SPLASH_STRUCT* splashes;
extern int nRipples;
extern RIPPLE_STRUCT* ripples;
extern SPLASH_SETUP splash_setup;
extern int nSpark;
extern SPARKS* spark;
extern int wibble;
extern int SplashCount;
extern int KillEverythingFlag;
extern int SmokeCountL;
extern int SmokeCountR;
extern int SmokeWeapon;
extern int SmokeWindX;
extern int SmokeWindZ;


#endif// TOMB4_GAME_EFFECT2_H
