#ifndef TOMB4_GAME_AIBITS_H
#define TOMB4_GAME_AIBITS_H
typedef enum ai_bits {
	GUARD = 1 << 0,
	AMBUSH = 1 << 1,
	PATROL1 = 1 << 2,
	MODIFY = 1 << 3,
	FOLLOW = 1 << 4
} ai_bits;
#endif // TOMB4_GAME_AIBITS_H
