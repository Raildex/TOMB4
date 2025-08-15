#ifndef TOMB4_GAME_COLLIDE_H
#define TOMB4_GAME_COLLIDE_H
typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;
typedef struct FLOOR_INFO FLOOR_INFO;
typedef struct PHD_3DPOS PHD_3DPOS;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct MESH_INFO MESH_INFO;
void ShiftItem(ITEM_INFO* item, COLL_INFO* coll);
int GetCollidedObjects(ITEM_INFO* item, int rad, int noInvisible, ITEM_INFO** StoredItems,int StoredItemsSize, MESH_INFO** StoredStatics, int StoredStaticsSize, int StoreLara);
void GenericDeadlyBoundingBoxCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void GenericSphereBoxCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CreatureCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void UpdateLaraRoom(ITEM_INFO* item, int height);
void LaraBaddieCollision(ITEM_INFO* l, COLL_INFO* coll);
void ObjectCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ObjectCollisionNoBigPush(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void TrapCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
int ItemPushLara(ITEM_INFO* item, ITEM_INFO* l, COLL_INFO* coll, int spaz, int BigPush);
int TestBoundsCollide(ITEM_INFO* item, ITEM_INFO* l, int rad);
int ItemPushLaraStatic(ITEM_INFO* l, short* bounds, PHD_3DPOS* pos, COLL_INFO* coll);
int TestLaraPosition(short* bounds, ITEM_INFO* item, ITEM_INFO* l);
void AlignLaraPosition(PHD_VECTOR* pos, ITEM_INFO* item, ITEM_INFO* l);
int MoveLaraPosition(PHD_VECTOR* v, ITEM_INFO* item, ITEM_INFO* l);
void StargateCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CogCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void GetCollisionInfo(COLL_INFO* coll, int x, int y, int z, short room_number, int hite);

extern short GlobalCollisionBounds[6];


#endif// TOMB4_GAME_COLLIDE_H
