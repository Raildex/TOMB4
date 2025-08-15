#ifndef TOMB4_GAME_AIOBJECT_H
#define TOMB4_GAME_AIOBJECT_H
#pragma pack(push, 1)
typedef struct AIOBJECT {
	short object_number;
	short room_number;
	int x;
	int y;
	int z;
	short trigger_flags;
	short flags;
	short y_rot;
	short box_number;
} AIOBJECT;
#pragma pack(pop)
#endif// TOMB4_GAME_AIOBJECT_H
