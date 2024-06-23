#pragma once

struct TEXTUREBUCKET;
struct MESH_DATA;
struct ROOM_INFO;
struct D3DTLBUMPVERTEX;
struct _D3DVECTOR;
void ProcessRoomDynamics(ROOM_INFO* r);
void ProcessRoomVertices(ROOM_INFO* r);
void ProcessRoomData(ROOM_INFO* r);
void InsertRoom(ROOM_INFO* r);
void CalcTriFaceNormal(_D3DVECTOR* p1, _D3DVECTOR* p2, _D3DVECTOR* p3, _D3DVECTOR* N);
void ProcessMeshData(long num_meshes);
void InitBuckets();
void DrawBucket(TEXTUREBUCKET* bucket);
void FindBucket(long tpage, D3DTLBUMPVERTEX** Vpp, long** nVtxpp);
void DrawBuckets();
void CreateVertexNormals(ROOM_INFO* r);

extern MESH_DATA** mesh_vtxbuf;
extern TEXTUREBUCKET Bucket[];
extern float clip_left;
extern float clip_top;
extern float clip_right;
extern float clip_bottom;
extern long bWaterEffect;
extern long num_level_meshes;

extern long water_color_R;
extern long water_color_G;
extern long water_color_B;
