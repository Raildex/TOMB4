#ifndef TOMB4_GAME_LARASURF_H
#define TOMB4_GAME_LARASURF_H

typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;


void lara_as_surfswim(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_surfback(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_surfleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_surfright(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_surftread(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_surfswim(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_surfback(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_surfleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_surfright(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_surftread(ITEM_INFO* item, COLL_INFO* coll);
void LaraSurface(ITEM_INFO* item, COLL_INFO* coll);
long LaraTestWaterClimbOut(ITEM_INFO* item, COLL_INFO* coll);
long LaraTestWaterStepOut(ITEM_INFO* item, COLL_INFO* coll);
void LaraSurfaceCollision(ITEM_INFO* item, COLL_INFO* coll);


#endif// TOMB4_GAME_LARASURF_H
