#ifndef TOMB4_GAME_JEEP_H
#define TOMB4_GAME_JEEP_H

typedef struct BIKEINFO BIKEINFO;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;


void InitialiseJeep(short item_number);
void DrawJeepExtras(ITEM_INFO* item);
void JeepExplode(ITEM_INFO* item);
void JeepCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
int GetCollisionAnim(ITEM_INFO* item, PHD_VECTOR* pos, BIKEINFO* vehicle);
int DoShift(ITEM_INFO* item, PHD_VECTOR* newPos, PHD_VECTOR* oldPos);
void JeepBaddieCollision(ITEM_INFO* item);
void JeepCollideStaticObjects(int x, int y, int z, short room_number, int height);
int JeepDynamics(ITEM_INFO* item);
void JeepControl(short item_number);
void JeepStart(ITEM_INFO* item, ITEM_INFO* l);
void JeepFireGrenade(ITEM_INFO* item);
void InitialiseEnemyJeep(short item_number);
void EnemyJeepControl(short item_number);


#endif// TOMB4_GAME_JEEP_H
