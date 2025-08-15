#ifndef TOMB4_GAME_ROPE_H
#define TOMB4_GAME_ROPE_H
#include "game/pendulum.h"
#include "game/ropestruct.h"

typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;


void DrawRopeList();
void ProjectRopePoints(ROPE_STRUCT* Rope);
PHD_VECTOR* Normalise(PHD_VECTOR* v);
void phd_GetMatrixAngles(int* m, short* dest);
void AlignLaraToRope(ITEM_INFO* l);
void SetPendulumVelocity(int x, int y, int z);
void RopeControl(short item_num);
void RopeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);

extern ROPE_STRUCT RopeList[64];
extern PENDULUM CurrentPendulum;
extern int nRope;


#endif// TOMB4_GAME_ROPE_H
