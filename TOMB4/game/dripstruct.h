#ifndef TOMB4_GAME_DRIPSTRUCT_H
#define TOMB4_GAME_DRIPSTRUCT_H
typedef struct DRIP_STRUCT {
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
} DRIP_STRUCT;
#endif // TOMB4_GAME_DRIPSTRUCT_H
