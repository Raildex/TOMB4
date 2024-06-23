#pragma once
#include "thread.h"
#include <cstdio>
struct TEXTURESTRUCT;
struct SPRITESTRUCT;
struct CHANGE_STRUCT;
struct RANGE_STRUCT;
struct AIOBJECT;
unsigned int __stdcall LoadLevel(void* name);
long S_LoadLevelFile(long num);
void FreeLevel();
bool FindCDDrive();
FILE* FileOpen(const char* name);
void FileClose(FILE* file);
long FileSize(FILE* file);
long LoadFile(const char* name, char** dest);
bool LoadTextures(long RTPages, long OTPages, long BTPages,FILE* f, char** FileData);
bool LoadRooms(char** data);
bool LoadObjects(char** data);
bool LoadSprites(char** data);
bool LoadCameras(char** data);
bool LoadSoundEffects(char** data);
bool LoadBoxes(char** data);
bool LoadAnimatedTextures(char** data);
bool LoadTextureInfos(char** data);
bool LoadItems(char** data);
bool LoadCinematic(char** data);
bool LoadAIInfo(char** data);
bool LoadSamples(FILE* file,char** data);
void S_GetUVRotateTextures();
void AdjustUV(long num);
bool Decompress(char* pDest, char* pCompressed, long compressedSize, long size);

extern TEXTURESTRUCT* textinfo;
extern SPRITESTRUCT* spriteinfo;
extern THREAD LevelLoadingThread;

extern TEXTURESTRUCT* AnimatingWaterfalls[3];
extern long AnimatingWaterfallsV[3];

extern CHANGE_STRUCT* changes;
extern RANGE_STRUCT* ranges;
extern AIOBJECT* AIObjects;
extern short* aranges;
extern short* frames;
extern short* commands;
extern short* floor_data;
extern short* mesh_base;
extern long nAnimUVRanges;
extern long number_cameras;
extern short nAIObjects;
