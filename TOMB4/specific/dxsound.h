#pragma once
#include <stdbool.h> 
typedef struct SAMPLE_BUFFER SAMPLE_BUFFER;
bool DXChangeOutputFormat(long nSamplesPerSec, bool force);
void DSChangeVolume(long num, long volume);
void DSAdjustPitch(long num, long pitch);
void DSAdjustPan(long num, long pan);
bool DXSetOutputFormat();
bool DXDSCreate();
bool S_ConvertSamples(unsigned char* data, long comp_size, long uncomp_size, long num,SAMPLE_BUFFER* buffers);
void DXStopSample(long num);
bool DSIsChannelPlaying(long num);
long DSGetFreeChannel();
long S_StartSample(SAMPLE_BUFFER* buffer, long volume, long pitch, long pan, unsigned long flags);
long CalcVolume(long volume);
void S_SoundStopAllSamples();
void S_SoundStopSample(long num);
long S_SoundPlaySample(SAMPLE_BUFFER* buffer, unsigned short volume, long pitch, short pan);
long S_SoundPlaySampleLooped(SAMPLE_BUFFER* buffer, unsigned short volume, long pitch, short pan);
void DXFreeSounds();
long S_SoundSampleIsPlaying(long num);
void S_SoundSetPanAndVolume(long num, short pan, unsigned short volume);
void S_SoundSetPitch(long num, long pitch);
void S_SetReverbType(long reverb);
void DXDSClose();

extern char* samples_buffer;
