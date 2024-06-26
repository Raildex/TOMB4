#pragma once
typedef struct ITEM_INFO ITEM_INFO;
void InitialiseBaboon(short item_number);
void BaboonControl(short item_number);
void FindCrowbarSwitch(ITEM_INFO* item, short switch_index);
void ReTriggerBaboon(short item_number);
void ExplodeBaboon(ITEM_INFO* item);
