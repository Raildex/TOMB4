#pragma once


struct WEAPON_INFO;
struct LARA_ARM;
struct GAME_VECTOR;
struct ITEM_INFO;

void InitialiseNewWeapon();
void LaraTargetInfo(WEAPON_INFO* winfo);
short* get_current_ammo_pointer(long weapon_type);
long FireWeapon(long weapon_type, ITEM_INFO* target, ITEM_INFO* src, short* angles);
void AimWeapon(WEAPON_INFO* winfo, LARA_ARM* arm);
void LaraGetNewTarget(WEAPON_INFO* winfo);
void HitTarget(ITEM_INFO* item, GAME_VECTOR* hitpos, long damage, long grenade);
long WeaponObject(long weapon_type);
long WeaponObjectMesh(long weapon_type);
void DoProperDetection(short item_number, long x, long y, long z, long xv, long yv, long zv);
void LaraGun();

extern WEAPON_INFO weapons[];
