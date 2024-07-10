#ifndef TOMB4_GAME_DOOR_H
#define TOMB4_GAME_DOOR_H
typedef struct DOORPOS_DATA DOORPOS_DATA;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;
void ShutThatDoor(DOORPOS_DATA* d);
void OpenThatDoor(DOORPOS_DATA* d);
void DoorControl(short item_number);
void DoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void PushPullKickDoorControl(short item_number);
void PushPullKickDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void DoubleDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void UnderwaterDoorCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void SequenceDoorControl(short item_number);


#endif// TOMB4_GAME_DOOR_H
