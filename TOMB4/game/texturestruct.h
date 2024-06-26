#ifndef TOMB4_GAME_TEXTURESTRUCT_H
#define TOMB4_GAME_TEXTURESTRUCT_H
#pragma pack(push, 1)
typedef struct TEXTURESTRUCT {
	unsigned short drawtype;
	unsigned short tpage;
	unsigned short flag;
	float u1;
	float v1;
	float u2;
	float v2;
	float u3;
	float v3;
	float u4;
	float v4;
} TEXTURESTRUCT;
#pragma pack(pop)
#endif // TOMB4_GAME_TEXTURESTRUCT_H
