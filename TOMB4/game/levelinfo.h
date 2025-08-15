#ifndef TOMB4_GAME_LEVELINFO_H
#define TOMB4_GAME_LEVELINFO_H


#include "game/camerainfo.h"
#include "game/spotcam.h"
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
typedef struct SPRITESTRUCT SPRITESTRUCT;
typedef struct AIOBJECT AIOBJECT;
LEVEL_INFO* CreateLevel();
void DestroyLevel(LEVEL_INFO* lvl);

BOX_INFO* GetBox(LEVEL_INFO* lvl, int box);
OBJECT_INFO* GetObjectInfo(LEVEL_INFO* lvl, int type);
STATIC_INFO* GetStaticObject(LEVEL_INFO* lvl, int type);
short* GetStaticObjectBounds(LEVEL_INFO*, int type);
ANIM_STRUCT* GetAnim(LEVEL_INFO* lvl, int anim);
int GetNumLevelItems(LEVEL_INFO* lvl);
ITEM_INFO* GetItem(LEVEL_INFO* lvl, int item);
FX_INFO* GetEffect(LEVEL_INFO* lvl, int fx);
MESH_INFO* GetStaticMesh(LEVEL_INFO* lvl, int mesh);
short** GetMeshPointer(LEVEL_INFO* lvl, int mesh);
short* GetMesh(LEVEL_INFO* lvl, int mesh);
short* GetMeshBase(LEVEL_INFO* lvl);
ROOM_INFO* GetRoom(LEVEL_INFO* lvl, int room);
int GetNumRooms(LEVEL_INFO* lvl);
short* GetFloorData(LEVEL_INFO* lvl, int index);
long* GetBone(LEVEL_INFO* lvl, int index);
CHANGE_STRUCT* GetAnimChange(LEVEL_INFO* lvl, int index);
RANGE_STRUCT* GetAnimRange(LEVEL_INFO* lvl,int index);
short* GetAnimCommand(LEVEL_INFO* lvl, int index);
short* GetAnimFrameBase(LEVEL_INFO* lvl);
short* GetAnimFrames(LEVEL_INFO* lvl, int offset);
short* GetZone(LEVEL_INFO* lvl,int zone,int flip);
unsigned short* GetOverlap(LEVEL_INFO* lvl,int overlap);
short GetItemNum(LEVEL_INFO* lvl, ITEM_INFO* i);
int GetNumBoxes(LEVEL_INFO* lvl);
SAMPLE_BUFFER* GetSampleBuffer(LEVEL_INFO* lvl, int num);
SAMPLE_INFO* GetSampleInfo(LEVEL_INFO* lvl, int num);
short* GetSampleLookup(LEVEL_INFO* lvl, int num);
HAL_TEXTURE* GetRendererTexture(LEVEL_INFO* lvl, int num);
int GetNumTextures(LEVEL_INFO* lvl);
TEXTURESTRUCT* GetTextInfo(LEVEL_INFO* lvl, int num);
HAL_TEXTURE* GetRendererBumpTexture(LEVEL_INFO* lvl, int num);
char HasRendererBumpTexture(LEVEL_INFO* lvl, int num);
SPRITESTRUCT* GetSpriteInfo(LEVEL_INFO* lvl, int num);
AIOBJECT* GetAIObject(LEVEL_INFO* lvl, int num);
int GetNumAnimTextureRanges(LEVEL_INFO* lvl);
short* GetAnimTextureRange(LEVEL_INFO* lvl, int num);
int GetNumAnimUVRanges(LEVEL_INFO* lvl);
int GetNumAIObjects(LEVEL_INFO* lvl);
OBJECT_VECTOR* GetFixedCamera(LEVEL_INFO* lvl, int num);
SPOTCAM* GetSpotCam(LEVEL_INFO* lvl, int num);
int GetNumSpotcams(LEVEL_INFO* lvl);
OBJECT_VECTOR* GetSoundEffect(LEVEL_INFO* lvl, int num);
int GetNumSoundEffects(LEVEL_INFO* lvl);
TEXTURESTRUCT* GetWaterfallTextInfos(LEVEL_INFO* lvl, int waterfall);
int GetWaterfallVCoordinate(LEVEL_INFO* lvl, int waterfall);
void* Allocate(LEVEL_INFO* lvl, int size, int count);
void Deallocate(LEVEL_INFO* lvl, void* memory);
extern LEVEL_INFO* currentLevel;
#endif// TOMB4_GAME_LEVELINFO_H
