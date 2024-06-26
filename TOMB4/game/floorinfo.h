#ifndef TOMB4_GAME_FLOORINFO_H
#define TOMB4_GAME_FLOORINFO_H
#pragma pack(push, 1)
typedef struct FLOOR_INFO {
	unsigned short index;
	unsigned short fx : 4;
	unsigned short box : 11;
	unsigned short stopper : 1;
	unsigned char pit_room;
	char floor;
	unsigned char sky_room;
	char ceiling;
} FLOOR_INFO;
#pragma pack(pop)
#endif // TOMB4_GAME_FLOORINFO_H
