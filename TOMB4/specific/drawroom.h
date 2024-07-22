#ifndef TOMB4_SPECIFIC_DRAWROOM_H
#define TOMB4_SPECIFIC_DRAWROOM_H

typedef struct LEVEL_INFO LEVEL_INFO;
typedef struct TEXTUREBUCKET TEXTUREBUCKET;
typedef struct MESH_DATA MESH_DATA;
typedef struct ROOM_INFO ROOM_INFO;
typedef struct D3DTLBUMPVERTEX D3DTLBUMPVERTEX;
typedef struct _D3DVECTOR _D3DVECTOR;
void ProcessRoomData(ROOM_INFO* r, short* data);
void InitBuckets();
void FindBucket(long tpage, D3DTLBUMPVERTEX** Vpp, long** nVtxpp);
void DrawBuckets();
void ProcessMesh(LEVEL_INFO* lvl, short* mesh_ptr, short* last_mesh_ptr, long i);
void S_InitRoomDraw();
extern MESH_DATA** mesh_vtxbuf;
extern float clip_left;
extern float clip_top;
extern float clip_right;
extern float clip_bottom;
extern long bWaterEffect;
extern long num_level_meshes;

extern long water_color_R;
extern long water_color_G;
extern long water_color_B;
extern short clipflags[8192];
extern float vert_wibble_table[32];


#endif// TOMB4_SPECIFIC_DRAWROOM_H
