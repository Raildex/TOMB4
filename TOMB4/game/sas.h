#pragma once

typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;


void InitialiseInjuredSas(short item_number);
void InjuredSasControl(short item_number);
void DragSASCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void InitialiseSas(short item_number);
void SasControl(short item_number);
