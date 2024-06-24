#ifndef TOMB4_GAME_SOUNDSLOT_H
#define TOMB4_GAME_SOUNDSLOT_H
#include "phdvector.h"
struct SoundSlot {
	long OrigVolume;
	long nVolume;
	long nPan;
	long nPitch;
	long nSampleInfo;
	unsigned long distance;
	PHD_VECTOR pos;
};
#endif // TOMB4_GAME_SOUNDSLOT_H
