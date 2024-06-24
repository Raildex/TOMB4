#ifndef ROOM_INFO_INCLUDED
#define ROOM_INFO_INCLUDED
struct _D3DVECTOR;
struct FLOOR_INFO;
struct LIGHTINFO;
struct MESH_INFO;
struct PCLIGHT_INFO;
struct IDirect3DVertexBuffer;
#pragma pack(push, 1)
struct ROOM_INFO {
	short* data;
	short* door;
	FLOOR_INFO* floor;
	LIGHTINFO* light;
	MESH_INFO* mesh;
	long x;
	long y;
	long z;
	long minfloor;
	long maxceiling;
	short x_size;
	short y_size;
	long ambient;
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
	long nVerts;
	long nWaterVerts;
	long nShoreVerts;
	IDirect3DVertexBuffer* SourceVB;
	short* FaceData;
	float posx;
	float posy;
	float posz;
	_D3DVECTOR* vnormals;
	_D3DVECTOR* fnormals;
	long* prelight;
	long* prelightwater;
	long watercalc;
	_D3DVECTOR* verts;
	long gt3cnt;
	long gt4cnt;
	PCLIGHT_INFO* pclight;
};
#pragma pack(pop)
#endif