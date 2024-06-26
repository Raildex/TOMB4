#ifndef MESH_INFO_INCLUDED
#define MESH_INFO_INCLUDED
#pragma pack(push, 1)
typedef struct MESH_INFO {
	long x;
	long y;
	long z;
	short y_rot;
	short shade;
	short Flags;
	short static_number;
} MESH_INFO;
#pragma pack(pop)
#endif