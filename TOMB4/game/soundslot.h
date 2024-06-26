#ifndef TOMB4_GAME_SOUNDSLOT_H
#define TOMB4_GAME_SOUNDSLOT_H
#include "game/phdvector.h"
typedef struct SoundSlot {
	long OrigVolume;
	long nVolume;
	long nPan;
	long nPitch;
	long nSampleInfo;
	unsigned long distance;
	PHD_VECTOR pos;
} SoundSlot;
#endif // TOMB4_GAME_SOUNDSLOT_H
