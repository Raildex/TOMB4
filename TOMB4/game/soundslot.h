#ifndef TOMB4_GAME_SOUNDSLOT_H
#define TOMB4_GAME_SOUNDSLOT_H
#include "game/phdvector.h"
typedef struct SoundSlot {
	int OrigVolume;
	int nVolume;
	int nPan;
	int nPitch;
	int nSampleInfo;
	unsigned int distance;
	PHD_VECTOR pos;
} SoundSlot;
#endif// TOMB4_GAME_SOUNDSLOT_H
