#ifndef TOMB4_GAME_LARACLMB_H
#define TOMB4_GAME_LARACLMB_H

typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;


void lara_as_climbstnc(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbright(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbright(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbing(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbdown(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_climbend(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbstnc(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbing(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_climbdown(ITEM_INFO* item, COLL_INFO* coll);
short GetClimbTrigger(int x, int y, int z, short room_number);
int LaraTestClimb(int x, int y, int z, int xfront, int zfront, int item_height, short item_room, int* shift);
int LaraTestClimbPos(ITEM_INFO* item, int front, int right, int origin, int height, int* shift);
int LaraTestClimbUpPos(ITEM_INFO* item, int front, int right, int* shift, int* ledge);
int LaraCheckForLetGo(ITEM_INFO* item, COLL_INFO* coll);
int LaraClimbLeftCornerTest(ITEM_INFO* item, COLL_INFO* coll);
int LaraClimbRightCornerTest(ITEM_INFO* item, COLL_INFO* coll);
void LaraDoClimbLeftRight(ITEM_INFO* item, COLL_INFO* coll, int result, int shift);


#endif// TOMB4_GAME_LARACLMB_H
