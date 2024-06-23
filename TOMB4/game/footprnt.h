#pragma once

struct ITEM_INFO;
struct FOOTPRINT;

void AddFootPrint(ITEM_INFO* item);
void S_DrawFootPrints();

extern FOOTPRINT FootPrint[];
extern long FootPrintNum;