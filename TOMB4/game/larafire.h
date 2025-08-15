#ifndef TOMB4_GAME_LARAFIRE_H
#define TOMB4_GAME_LARAFIRE_H
#include "game/weaponinfo.h"

typedef struct LARA_ARM LARA_ARM;
typedef struct GAME_VECTOR GAME_VECTOR;
typedef struct ITEM_INFO ITEM_INFO;

void InitialiseNewWeapon();
void LaraTargetInfo(WEAPON_INFO* winfo);
short* get_current_ammo_pointer(int weapon_type);
int FireWeapon(int weapon_type, ITEM_INFO* target, ITEM_INFO* src, short* angles);
void AimWeapon(WEAPON_INFO* winfo, LARA_ARM* arm);
void LaraGetNewTarget(WEAPON_INFO* winfo);
void HitTarget(ITEM_INFO* item,GAME_VECTOR* src, GAME_VECTOR* hitpos, int damage, int grenade);
int WeaponObject(int weapon_type);
int WeaponObjectMesh(int weapon_type);
void DoProperDetection(short item_number, int x, int y, int z, int xv, int yv, int zv);
void LaraGun();

extern WEAPON_INFO weapons[9];


#endif// TOMB4_GAME_LARAFIRE_H
