#ifndef OBJECT_VECTOR_INCLUDED
#define OBJECT_VECTOR_INCLUDED
#pragma pack(push, 1)
typedef struct OBJECT_VECTOR {
	long x;
	long y;
	long z;
	short data;
	short flags;
} OBJECT_VECTOR;
#pragma pack(pop)
#endif