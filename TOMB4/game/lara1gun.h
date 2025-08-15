#ifndef TOMB4_GAME_LARA1GUN_H
#define TOMB4_GAME_LARA1GUN_H

typedef struct ITEM_INFO ITEM_INFO;
typedef struct PHD_3DPOS PHD_3DPOS;


void DoGrenadeDamageOnBaddie(ITEM_INFO* baddie, ITEM_INFO* item);
void FireCrossbow(PHD_3DPOS* pos);
void draw_shotgun_meshes(int weapon_type);
void undraw_shotgun_meshes(int weapon_type);
void ready_shotgun(int weapon_type);
void FireShotgun();
void FireGrenade();
void AnimateShotgun(int weapon_type);
void RifleHandler(int weapon_type);
void CrossbowHitSwitchType78(ITEM_INFO* item, ITEM_INFO* target, int MustHitLastNode);
void TriggerUnderwaterExplosion(ITEM_INFO* item, int vehicle);
void draw_shotgun(int weapon_type);
void undraw_shotgun(int weapon_type);
void ControlCrossbow(short item_number);
void ControlGrenade(short item_number);


#endif// TOMB4_GAME_LARA1GUN_H
