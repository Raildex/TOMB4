#ifndef TOMB4_SPECIFIC_OUTPUT_H
#define TOMB4_SPECIFIC_OUTPUT_H
#include "game/languages.h"
typedef struct MESH_DATA MESH_DATA;
typedef struct ROOM_INFO ROOM_INFO;
void ProcessObjectMeshVertices(MESH_DATA* mesh);
void ProcessStaticMeshVertices(MESH_DATA* mesh);
void ProcessTrainMeshVertices(MESH_DATA* mesh);
void ProcessPickupMeshVertices(MESH_DATA* mesh);
void phd_PutPolygons(short* objptr, int clip);
void phd_PutPolygons_train(short* objptr, int x);
void S_InsertRoom(ROOM_INFO* r);
void RenderLoadPic(int unused);
void S_InitialisePolyList();
void phd_PutPolygonsPickup(short* objptr, float x, float y, int color);
void phd_PutPolygonSkyMesh(short* objptr, int clipstatus);
void S_DrawPickup(short object_number);
int S_GetObjectInfoBounds(short* bounds);
void do_boot_screen(languages language);
void S_AnimateTextures(int n);
int S_DumpScreen();
void S_OutputPolyList();
void StashSkinVertices(int node);
void SkinVerticesToScratch(int node);
int GetRenderScale(int unit);
int GetFixedScale(int unit);

extern int GlobalAlpha;
extern int GlobalAmbient;


#endif// TOMB4_SPECIFIC_OUTPUT_H
