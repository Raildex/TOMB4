#ifndef TOMB4_GAME_FOOTPRNT_H
#define TOMB4_GAME_FOOTPRNT_H

typedef struct ITEM_INFO ITEM_INFO;
typedef struct FOOTPRINT FOOTPRINT;

void AddFootPrint(ITEM_INFO* item);
void S_DrawFootPrints();

extern FOOTPRINT FootPrint[];
extern long FootPrintNum;


#endif// TOMB4_GAME_FOOTPRNT_H
