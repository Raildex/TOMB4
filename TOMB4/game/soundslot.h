#ifndef SOUNDSLOT_INCLUDED
#define SOUNDSLOT_INCLUDED
#include "phdvector.h"
struct SoundSlot
{
	long OrigVolume;
	long nVolume;
	long nPan;
	long nPitch;
	long nSampleInfo;
	unsigned long distance;
	PHD_VECTOR pos;
};
#endif