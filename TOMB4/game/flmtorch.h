#pragma once

typedef struct COLL_INFO COLL_INFO;
typedef struct ITEM_INFO ITEM_INFO;

void TriggerTorchFlame(short item_number, long node);
void FireCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void DoFlameTorch();
void GetFlameTorch();
void FlameTorchControl(short item_number);
