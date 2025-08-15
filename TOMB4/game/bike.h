#ifndef TOMB4_GAME_BIKE_H
#define TOMB4_GAME_BIKE_H

typedef struct COLL_INFO COLL_INFO;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct PHD_VECTOR PHD_VECTOR;

void InitialiseBike(short item_number);
void DrawBikeExtras(ITEM_INFO* item);
void TriggerBikeBeam(ITEM_INFO* item);
int GetOnBike(short item_number, COLL_INFO* coll);
void DrawBikeBeam(ITEM_INFO* item);
void BikeExplode(ITEM_INFO* item);
void AnimateBike(ITEM_INFO* item, int hitWall, int killed);
void BikeStart(ITEM_INFO* item, ITEM_INFO* l);
int TestHeight(ITEM_INFO* item, int z, int x, PHD_VECTOR* pos);
void BikeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
int BikeBaddieCollision(ITEM_INFO* bike);
void BikeCollideStaticObjects(int x, int y, int z, short room_number, int height);
int BikeDynamics(ITEM_INFO* item);
void BikeControl(short item_number);


#endif// TOMB4_GAME_BIKE_H
