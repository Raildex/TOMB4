#ifndef TOMB4_GAME_COLLIDE_H
#define TOMB4_GAME_COLLIDE_H
typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;
typedef struct FLOOR_INFO FLOOR_INFO;
typedef struct PHD_3DPOS PHD_3DPOS;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct MESH_INFO MESH_INFO;
void ShiftItem(ITEM_INFO* item, COLL_INFO* coll);
long GetCollidedObjects(ITEM_INFO* item, long rad, long noInvisible, ITEM_INFO** StoredItems,long StoredItemsSize, MESH_INFO** StoredStatics, long StoredStaticsSize, long StoreLara);
void GenericDeadlyBoundingBoxCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void GenericSphereBoxCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CreatureCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void UpdateLaraRoom(ITEM_INFO* item, long height);
void LaraBaddieCollision(ITEM_INFO* l, COLL_INFO* coll);
void ObjectCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ObjectCollisionNoBigPush(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void TrapCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long ItemPushLara(ITEM_INFO* item, ITEM_INFO* l, COLL_INFO* coll, long spaz, long BigPush);
long TestBoundsCollide(ITEM_INFO* item, ITEM_INFO* l, long rad);
long ItemPushLaraStatic(ITEM_INFO* l, short* bounds, PHD_3DPOS* pos, COLL_INFO* coll);
long TestLaraPosition(short* bounds, ITEM_INFO* item, ITEM_INFO* l);
void AlignLaraPosition(PHD_VECTOR* pos, ITEM_INFO* item, ITEM_INFO* l);
long MoveLaraPosition(PHD_VECTOR* v, ITEM_INFO* item, ITEM_INFO* l);
void StargateCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CogCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void GetCollisionInfo(COLL_INFO* coll, long x, long y, long z, short room_number, long hite);

extern short GlobalCollisionBounds[6];


#endif// TOMB4_GAME_COLLIDE_H
