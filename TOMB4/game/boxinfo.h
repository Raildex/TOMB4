#ifndef TOMB4_GAME_BOXINFO_H
#define TOMB4_GAME_BOXINFO_H
#pragma pack(push, 1)
typedef struct BOX_INFO {
	unsigned char left;
	unsigned char right;
	unsigned char top;
	unsigned char bottom;
	short height;
	short overlap_index;
} BOX_INFO;
#pragma pack(pop)
#endif // TOMB4_GAME_BOXINFO_H
