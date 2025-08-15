#ifndef TOMB4_GAME_ROOMINFO_H
#define TOMB4_GAME_ROOMINFO_H
#include "game/control.h"
#include "game/fvector.h"
#include "polyface.h"
typedef struct _D3DVECTOR _D3DVECTOR;
typedef struct FLOOR_INFO FLOOR_INFO;
typedef struct LIGHTINFO LIGHTINFO;
typedef struct MESH_INFO MESH_INFO;
typedef struct PCLIGHT_INFO PCLIGHT_INFO;
typedef struct IDirect3DVertexBuffer IDirect3DVertexBuffer;
#pragma pack(push, 1)
typedef struct ROOM_INFO {
	short* door;
	FLOOR_INFO* floor;
	LIGHTINFO* light;
	MESH_INFO* mesh;
	int x;
	int y;
	int z;
	int minfloor;
	int maxceiling;
	short x_size;
	short y_size;
	int ambient;
	short num_lights;
	short num_meshes;
	unsigned char ReverbType;
	unsigned char FlipNumber;
	char MeshEffect;
	char bound_active;
	short left;
	short right;
	short top;
	short bottom;
	short test_left;
	short test_right;
	short test_top;
	short test_bottom;
	short item_number;
	short fx_number;
	short flipped_room;
	unsigned short flags;
	int nVerts;
	int nWaterVerts;
	int nShoreVerts;
	POLYFACE3* tris;
	POLYFACE4* quads;
	float posx;
	float posy;
	float posz;
	FVECTOR* vnormals;
	int* prelight;
	int* prelightwater;
	int watercalc;
	PHD_VECTOR* verts;
	int gt3cnt;
	int gt4cnt;
	PCLIGHT_INFO* pclight;
} ROOM_INFO;
#pragma pack(pop)
#endif// TOMB4_GAME_ROOMINFO_H
