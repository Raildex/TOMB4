#ifndef TOMB4_GAME_LEVELINFO_H
#define TOMB4_GAME_LEVELINFO_H

#include "roominfo.h"
struct LEVEL_INFO;
struct OBJECT_INFO;
struct ANIM_STRUCT;
struct FX_INFO;
struct STATIC_INFO;
struct MESH_INFO;
struct ITEM_INFO;
LEVEL_INFO* CreateLevel();
void LoadLevel(LEVEL_INFO* lvl, char* file);
void DestroyLevel(LEVEL_INFO* lvl);

OBJECT_INFO* GetObjectInfo(LEVEL_INFO* lvl, long type);
STATIC_INFO* GetStaticObject(LEVEL_INFO* lvl, long type);
short* GetStaticObjectBounds(LEVEL_INFO*, long type);
ANIM_STRUCT* GetAnim(LEVEL_INFO* lvl, long anim);
ITEM_INFO* GetItem(LEVEL_INFO* lvl, long item);
FX_INFO* GetEffect(LEVEL_INFO* lvl, long fx);
MESH_INFO* GetMesh(LEVEL_INFO* lvl, long mesh);
ROOM_INFO* GetRoom(LEVEL_INFO* lvl, long room);
long GetNumRooms(LEVEL_INFO* lvl);
short* GetFloorData(LEVEL_INFO* lvl, long index);
bool LoadRooms(char** FileData, LEVEL_INFO* lvl);
extern LEVEL_INFO* currentLevel;
#endif// TOMB4_GAME_LEVELINFO_H
