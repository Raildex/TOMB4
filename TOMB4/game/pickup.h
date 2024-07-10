#ifndef TOMB4_GAME_PICKUP_H
#define TOMB4_GAME_PICKUP_H

typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;


void SarcophagusCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void KeyHoleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void PuzzleDoneCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);
void PuzzleDone(ITEM_INFO* item, short item_number);
void AnimatingPickUp(short item_number);
short* FindPlinth(ITEM_INFO* item);
long KeyTrigger(short item_num);
long PickupTrigger(short item_num);
void RegeneratePickups();
void PickUpCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void PuzzleHoleCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);

extern unsigned char NumRPickups;
extern unsigned char RPickups[16];
extern char KeyTriggerActive;


#endif// TOMB4_GAME_PICKUP_H
