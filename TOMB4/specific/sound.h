#ifndef TOMB4_SPECIFIC_SOUND_H
#define TOMB4_SPECIFIC_SOUND_H
typedef struct SAMPLE_BUFFER SAMPLE_BUFFER;
long S_ConvertSamples(unsigned char* data, long comp_size, long uncomp_size, long num, SAMPLE_BUFFER* buffers);
long S_StartSample(SAMPLE_BUFFER* buffer, long volume, long pitch, long pan, unsigned long flags);
void S_SoundStopAllSamples();
void S_SoundStopSample(long num);
long S_SoundPlaySample(SAMPLE_BUFFER* buffer, unsigned short volume, long pitch, short pan);
long S_SoundPlaySampleLooped(SAMPLE_BUFFER* buffer, unsigned short volume, long pitch, short pan);
long S_SoundSampleIsPlaying(long num);
void S_SoundSetPanAndVolume(long num, short pan, unsigned short volume);
void S_SoundSetPitch(long num, long pitch);
void S_SetReverbType(long reverb);

#endif// TOMB4_SPECIFIC_SOUND_H
