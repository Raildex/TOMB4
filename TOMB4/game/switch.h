#ifndef TOMB4_GAME_SWITCH_H
#define TOMB4_GAME_SWITCH_H
#include "game/phdvector.h"

typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;


void FullBlockSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long SwitchTrigger(short item_number, short timer);
long GetSwitchTrigger(ITEM_INFO* item, short* ItemNos, long AttatchedToSwitch);
void TestTriggersAtXYZ(long x, long y, long z, short room_number, short heavy, short flags);
void SwitchControl(short item_number);
void SwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SwitchCollision2(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SwitchType78Collision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void UnderwaterSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void PulleyCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void TurnSwitchControl(short item_number);
void TurnSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void RailSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void JumpSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CrowbarSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void FullBlockSwitchControl(short item_number);
void CogSwitchControl(short item_number);
void CogSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);

extern PHD_VECTOR OldPickupPos;
extern unsigned char CurrentSequence;
extern unsigned char Sequences[3];
extern unsigned char SequenceUsed[6];
extern unsigned char SequenceResults[3][3][3];


#endif// TOMB4_GAME_SWITCH_H
