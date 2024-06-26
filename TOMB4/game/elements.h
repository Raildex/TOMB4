#pragma once

typedef struct COLL_INFO COLL_INFO;
typedef struct ITEM_INFO ITEM_INFO;

void ElementPuzzleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void InitialiseElementPuzzle(short item_number);
void ControlElementPuzzle(short item_number);
