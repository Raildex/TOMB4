#ifndef TOMB4_GAME_PEOPLE_H
#define TOMB4_GAME_PEOPLE_H
#include "game/phdvector.h"

typedef struct ITEM_INFO ITEM_INFO;
typedef struct AI_INFO AI_INFO;
typedef struct BITE_INFO BITE_INFO;


short GunShot(long x, long y, long z, short speed, short yrot,PHD_VECTOR dir, short room_number);
short GunHit(long x, long y, long z, short speed, short yrot,PHD_VECTOR dir, short room_number);
short GunMiss(long x, long y, long z, short speed, short yrot,PHD_VECTOR dir, short room_number);
long TargetVisible(ITEM_INFO* item, AI_INFO* info);
long Targetable(ITEM_INFO* item, AI_INFO* info);
long ShotLara(ITEM_INFO* item, AI_INFO* info, BITE_INFO* gun, short extra_rotation, long damage);


#endif// TOMB4_GAME_PEOPLE_H
