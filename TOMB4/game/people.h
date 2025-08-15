#ifndef TOMB4_GAME_PEOPLE_H
#define TOMB4_GAME_PEOPLE_H
#include "game/phdvector.h"

typedef struct ITEM_INFO ITEM_INFO;
typedef struct AI_INFO AI_INFO;
typedef struct BITE_INFO BITE_INFO;


short GunShot(int x, int y, int z, short speed, short yrot,PHD_VECTOR dir, short room_number);
short GunHit(int x, int y, int z, short speed, short yrot,PHD_VECTOR dir, short room_number);
short GunMiss(int x, int y, int z, short speed, short yrot,PHD_VECTOR dir, short room_number);
int TargetVisible(ITEM_INFO* item, AI_INFO* info);
int Targetable(ITEM_INFO* item, AI_INFO* info);
int ShotLara(ITEM_INFO* item, AI_INFO* info, BITE_INFO* gun, short extra_rotation, int damage);


#endif// TOMB4_GAME_PEOPLE_H
