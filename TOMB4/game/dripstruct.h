#ifndef TOMB4_GAME_DRIPSTRUCT_H
#define TOMB4_GAME_DRIPSTRUCT_H
struct DRIP_STRUCT {
	long x;
	long y;
	long z;
	unsigned char On;
	unsigned char R;
	unsigned char G;
	unsigned char B;
	short Yvel;
	unsigned char Gravity;
	unsigned char Life;
	short RoomNumber;
	unsigned char Outside;
	unsigned char Pad;
};
#endif // TOMB4_GAME_DRIPSTRUCT_H
