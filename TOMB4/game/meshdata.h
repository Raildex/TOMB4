#ifndef TOMB4_GAME_MESHDATA_H
#define TOMB4_GAME_MESHDATA_H
typedef struct IDirect3DVertexBuffer IDirect3DVertexBuffer;
typedef struct _D3DVECTOR _D3DVECTOR;
typedef struct MESH_DATA {
	short x;
	short y;
	short z;
	short r;
	short flags;
	short nVerts;
	short nNorms;
	short ngt4;
	short* gt4;
	short ngt3;
	short* gt3;
	long* prelight;
	IDirect3DVertexBuffer* SourceVB;
	_D3DVECTOR* Normals;
} MESH_DATA;
#endif // TOMB4_GAME_MESHDATA_H
