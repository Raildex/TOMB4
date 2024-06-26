#pragma once

typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;


void InitialiseMovingBlock(short item_number);
void MovableBlock(short item_number);
void MovableBlockCollision(short item_number, ITEM_INFO* laraitem, COLL_INFO* coll);
void InitialisePlanetEffect(short item_number);
void ControlPlanetEffect(short item_number);
void DrawPlanetEffect(ITEM_INFO* item);
