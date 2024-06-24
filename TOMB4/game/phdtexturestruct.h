#ifndef TOMB4_GAME_PHDTEXTURESTRUCT_H
#define TOMB4_GAME_PHDTEXTURESTRUCT_H
#pragma pack(push, 1)
struct PHDTEXTURESTRUCT {
	unsigned short drawtype;
	unsigned short tpage;
	unsigned short flag;
	unsigned short u1;
	unsigned short v1;
	unsigned short u2;
	unsigned short v2;
	unsigned short u3;
	unsigned short v3;
	unsigned short u4;
	unsigned short v4;
	unsigned long xoff;
	unsigned long yoff;
	unsigned long width;
	unsigned long height;
};
#pragma pack(pop)
#endif // TOMB4_GAME_PHDTEXTURESTRUCT_H
