#ifndef TOMB4_GAME_LOT_H
#define TOMB4_GAME_LOT_H
typedef struct LOT_INFO LOT_INFO;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct CREATURE_INFO CREATURE_INFO;
void InitialiseLOTarray(long allocmem);
void DisableBaddieAI(short item_number);
void ClearLOT(LOT_INFO* lot);
void CreateZone(ITEM_INFO* item);
void InitialiseSlot(short item_number, long slot);
long EnableBaddieAI(short item_number, long Always);

extern CREATURE_INFO* baddie_slots;


#endif// TOMB4_GAME_LOT_H
