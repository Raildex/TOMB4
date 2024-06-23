#pragma once
struct GAME_VECTOR;
struct SHATTER_ITEM;
struct MESH_INFO;
struct DEBRIS_STRUCT;
void UpdateDebris();
void TriggerDebris(GAME_VECTOR* pos, void* TextInfo, short* Offsets, long* Vels, short rgb);
long GetFreeDebris();
void ShatterObject(SHATTER_ITEM* shatter_item, MESH_INFO* StaticMesh, short Num, short RoomNumber, long NoXZVel);

extern DEBRIS_STRUCT debris[];
extern long next_debris;
extern short DebrisFlags;
