#ifndef TOMB4_GAME_PHDSPRITESTRUCT_H
#define TOMB4_GAME_PHDSPRITESTRUCT_H
#pragma pack(push, 1)
typedef struct PHDSPRITESTRUCT {
	unsigned short tpage;
	unsigned short offset;
	unsigned short width;
	unsigned short height;
	short x1;
	short y1;
	short x2;
	short y2;
} PHDSPRITESTRUCT;
#pragma pack(pop)
#endif // TOMB4_GAME_PHDSPRITESTRUCT_H
