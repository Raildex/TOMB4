#ifndef TOMB4_SPECIFIC_FILE_H
#define TOMB4_SPECIFIC_FILE_H
#include "specific/thread.h"
#include "texture.h"
#include <stdio.h>
#include <windows.h>
typedef struct TEXTURESTRUCT TEXTURESTRUCT;
typedef struct SPRITESTRUCT SPRITESTRUCT;
typedef struct CHANGE_STRUCT CHANGE_STRUCT;
typedef struct RANGE_STRUCT RANGE_STRUCT;
typedef struct AIOBJECT AIOBJECT;
typedef struct LEVEL_INFO LEVEL_INFO;
long S_LoadLevelFile(long num);
void FreeLevel();
long FindCDDrive();
FILE* FileOpen(const char* name);
void FileClose(FILE* file);
long FileSize(FILE* file);
long LoadFile(const char* name, char** dest);
char LoadTextures(TEXTURE_FORMAT fmt, FILE* f, LEVEL_INFO* lvl);
char LoadRooms(char** data,LEVEL_INFO* lvl);
char LoadObjects(char** data,LEVEL_INFO* lvl);
char LoadSprites(char** data, LEVEL_INFO* lvl);
char LoadCameras(char** data, LEVEL_INFO* lvl);
char LoadSoundEffects(char** data, LEVEL_INFO* lvl);
char LoadAnimatedTextures(char** data, LEVEL_INFO* lvl);
char LoadTextureInfos(char** data, LEVEL_INFO* lvl);
char LoadBoxes(char **data,LEVEL_INFO* lvl);
char LoadItems(char** data, LEVEL_INFO* lvl);
char LoadCinematic(char** data, LEVEL_INFO* lvl);
char LoadAIInfo(char** data, LEVEL_INFO* lvl);
char LoadSamples(FILE* file, char** data, LEVEL_INFO* lvl);
void S_GetUVRotateTextures(LEVEL_INFO* lvl);
char S_Decompress(char* pDest, char* pCompressed, long compressedSize, long size);

extern THREAD* LevelLoadingThread;


#endif// TOMB4_SPECIFIC_FILE_H
