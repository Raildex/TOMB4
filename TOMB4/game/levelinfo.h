#ifndef TOMB4_GAME_LEVELINFO_H
#define TOMB4_GAME_LEVELINFO_H


typedef struct HAL_TEXTURE HAL_TEXTURE;
typedef struct TEXTURESTRUCT TEXTURESTRUCT;
typedef struct ROOM_INFO ROOM_INFO;
typedef struct CHANGE_STRUCT CHANGE_STRUCT;
typedef struct RANGE_STRUCT RANGE_STRUCT;
typedef struct LEVEL_INFO LEVEL_INFO;
typedef struct OBJECT_INFO OBJECT_INFO;
typedef struct ANIM_STRUCT ANIM_STRUCT;
typedef struct FX_INFO FX_INFO;
typedef struct STATIC_INFO STATIC_INFO;
typedef struct MESH_INFO MESH_INFO;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct BOX_INFO BOX_INFO;
typedef struct SAMPLE_BUFFER SAMPLE_BUFFER;
typedef struct SAMPLE_INFO SAMPLE_INFO;
typedef struct TEXTURE TEXTURE;
LEVEL_INFO* CreateLevel();
void DestroyLevel(LEVEL_INFO* lvl);

BOX_INFO* GetBox(LEVEL_INFO* lvl, long box);
OBJECT_INFO* GetObjectInfo(LEVEL_INFO* lvl, long type);
STATIC_INFO* GetStaticObject(LEVEL_INFO* lvl, long type);
short* GetStaticObjectBounds(LEVEL_INFO*, long type);
ANIM_STRUCT* GetAnim(LEVEL_INFO* lvl, long anim);
long GetNumLevelItems(LEVEL_INFO* lvl);
ITEM_INFO* GetItem(LEVEL_INFO* lvl, long item);
FX_INFO* GetEffect(LEVEL_INFO* lvl, long fx);
MESH_INFO* GetStaticMesh(LEVEL_INFO* lvl, long mesh);
short** GetMeshPointer(LEVEL_INFO* lvl, long mesh);
short* GetMesh(LEVEL_INFO* lvl, long mesh);
short* GetMeshBase(LEVEL_INFO* lvl);
ROOM_INFO* GetRoom(LEVEL_INFO* lvl, long room);
long GetNumRooms(LEVEL_INFO* lvl);
short* GetFloorData(LEVEL_INFO* lvl, long index);
long* GetBone(LEVEL_INFO* lvl, long index);
CHANGE_STRUCT* GetAnimChange(LEVEL_INFO* lvl, long index);
RANGE_STRUCT* GetAnimRange(LEVEL_INFO* lvl,long index);
short* GetAnimCommand(LEVEL_INFO* lvl, long index);
short* GetAnimFrameBase(LEVEL_INFO* lvl);
short* GetAnimFrames(LEVEL_INFO* lvl, long offset);
short* GetZone(LEVEL_INFO* lvl,long zone,long flip);
unsigned short* GetOverlap(LEVEL_INFO* lvl,long overlap);
FX_INFO* GetEffect(LEVEL_INFO* lvl, long fx);
short GetItemNum(LEVEL_INFO* lvl, ITEM_INFO* i);
long GetNumBoxes(LEVEL_INFO* lvl);
SAMPLE_BUFFER* GetSampleBuffer(LEVEL_INFO* lvl, long num);
SAMPLE_INFO* GetSampleInfo(LEVEL_INFO* lvl, long num);
short* GetSampleLookup(LEVEL_INFO* lvl, long num);
HAL_TEXTURE* GetRendererTexture(LEVEL_INFO* lvl, long num);
long GetNumTextures(LEVEL_INFO* lvl);
TEXTURESTRUCT* GetTextInfo(LEVEL_INFO* lvl, long num);
HAL_TEXTURE* GetRendererBumpTexture(LEVEL_INFO* lvl, long num);
char HasRendererBumpTexture(LEVEL_INFO* lvl, long num);
extern LEVEL_INFO* currentLevel;
#endif// TOMB4_GAME_LEVELINFO_H
