#pragma once
#include "debrisstruct.h"
typedef struct GAME_VECTOR GAME_VECTOR;
typedef struct SHATTER_ITEM SHATTER_ITEM;
typedef struct MESH_INFO MESH_INFO;
void UpdateDebris();
void TriggerDebris(GAME_VECTOR* pos, void* TextInfo, short* Offsets, long* Vels, short rgb);
long GetFreeDebris();
void ShatterObject(SHATTER_ITEM* shatter_item, MESH_INFO* StaticMesh, short Num, short RoomNumber, long NoXZVel);

extern DEBRIS_STRUCT debris[];
extern long next_debris;
extern short DebrisFlags;
