#ifndef TOMB4_GAME_SPRITESTRUCT_H
#define TOMB4_GAME_SPRITESTRUCT_H
#pragma pack(push, 1)
struct SPRITESTRUCT {
	unsigned short tpage;
	unsigned short offset;
	unsigned short width;
	unsigned short height;
	float x1; // left
	float y1; // top
	float x2; // right
	float y2; // bottom
};
#pragma pack(pop)
#endif // TOMB4_GAME_SPRITESTRUCT_H
