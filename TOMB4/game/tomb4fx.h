#ifndef TOMB4_GAME_TOMB4FX_H
#define TOMB4_GAME_TOMB4FX_H
#include "game/nodeoffsetinfo.h"
#include "game/weapontypes.h"
typedef struct LIGHTNING_STRUCT LIGHTNING_STRUCT;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct SVECTOR SVECTOR;
typedef struct PHD_3DPOS PHD_3DPOS;
typedef struct NODEOFFSET_INFO NODEOFFSET_INFO;
typedef struct LIGHTNING_STRUCT LIGHTNING_STRUCT;
typedef struct GUNSHELL_STRUCT GUNSHELL_STRUCT;
typedef struct DRIP_STRUCT DRIP_STRUCT;
typedef struct SMOKE_SPARKS SMOKE_SPARKS;
typedef struct BUBBLE_STRUCT BUBBLE_STRUCT;
typedef struct SHOCKWAVE_STRUCT SHOCKWAVE_STRUCT;
typedef struct FIRE_SPARKS FIRE_SPARKS;
typedef struct BLOOD_STRUCT BLOOD_STRUCT;
typedef struct GUNFLASH_STRUCT GUNFLASH_STRUCT;
typedef struct FIRE_LIST FIRE_LIST;

LIGHTNING_STRUCT* TriggerLightning(PHD_VECTOR* s, PHD_VECTOR* d, char variation, long rgb, unsigned char flags, unsigned char size, unsigned char segments);
long ExplodingDeath2(short item_number, long mesh_bits, short Flags);
void DrawGunshells();
void TriggerGunSmoke(long x, long y, long z, long xVel, long yVel, long zVel, long notLara, long weaponType, long shade);
void LaraBubbles(ITEM_INFO* item);
void UpdateDrips();
void TriggerGlobalStaticFlame();
void TriggerGlobalFireFlame();
void keep_those_fires_burning();
void UpdateFireSparks();
void ClearFires();
void AddFire(long x, long y, long z, long size, short room_number, short fade);
void S_DrawFires();
DRIP_STRUCT* GetFreeDrip();
void UpdateSmokeSparks();
void TriggerShatterSmoke(long x, long y, long z);
void DrawLensFlares(ITEM_INFO* item);
void DrawWeaponMissile(ITEM_INFO* item);
void TriggerGunShell(short leftright, short objnum, weapon_types weapon);
void UpdateGunShells();
void TriggerSmallSplash(long x, long y, long z, long num);
void TriggerGunflash(PHD_VECTOR* pos);
void SetGunFlash(short weapon);
void DrawGunflashes();
void UpdateBlood();
void TriggerBlood(long x, long y, long z, long angle, long num);
void CreateBubble(PHD_3DPOS* pos, short room_number, long size, long biggest);
void UpdateBubbles();
void TriggerLaraDrips();
SHOCKWAVE_STRUCT* GetFreeShockwave();
void TriggerShockwave(PHD_VECTOR* pos, long InnerOuterRads, long speed, long bgrl, long XRotFlags);
void TriggerShockwaveHitEffect(long x, long y, long z, long rgb, short dir, long speed);
void UpdateShockwaves();
void UpdateLightning();
long LSpline(long x, long* knots, long nk);
void CalcLightningSpline(PHD_VECTOR* pos, PHD_VECTOR* dest, LIGHTNING_STRUCT* lptr);
void TriggerLightningGlow(long x, long y, long z, long rgb);
void TriggerFlashSmoke(long x, long y, long z, short room_number);
void S_DrawSparks();
void SetFadeClip(short height, short speed);
void UpdateFadeClip();
void SetScreenFadeOut(short speed, short back);
void SetScreenFadeIn(short speed);
void Fade();
SMOKE_SPARKS* GetFreeSmokeSpark();
extern NODEOFFSET_INFO NodeOffsets[];
extern long nLightnings;
extern LIGHTNING_STRUCT* Lightning;
extern long nGunshells;
extern GUNSHELL_STRUCT* Gunshells;
extern long nDrips;
extern DRIP_STRUCT* Drips;
extern long nSmokeSparks;
extern SMOKE_SPARKS* smoke_spark;
extern long nBubbles;
extern BUBBLE_STRUCT* Bubbles;
extern long nShockWaves;
extern SHOCKWAVE_STRUCT* ShockWaves;
extern long nFireSparks;
extern FIRE_SPARKS* fire_spark;
extern long nBlood;
extern BLOOD_STRUCT* blood;
extern long nGunflashes;
extern GUNFLASH_STRUCT* Gunflashes;
extern long nFires;
extern FIRE_LIST* fires;
extern short FlashFadeR;
extern short FlashFadeG;
extern short FlashFadeB;
extern short FlashFader;
extern short ScreenFade;
extern short dScreenFade;
extern short ScreenFadeBack;
extern short ScreenFadedOut;
extern short ScreenFading;
extern short FadeScreenHeight;
extern short DestFadeScreenHeight;
extern short FadeClipSpeed;
extern short ScreenFadeSpeed;
extern char tsv_buffer[16384];


#endif// TOMB4_GAME_TOMB4FX_H
