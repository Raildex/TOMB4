#ifndef DS_SAMPLE_INCLUDED
#define DS_SAMPLE_INCLUDED
struct DS_SAMPLE
{
	LPDIRECTSOUNDBUFFER buffer;
	long frequency;
	long playing;
};
#endif