#ifndef TOMB4_SPECIFIC_SOUND_H
#define TOMB4_SPECIFIC_SOUND_H
typedef struct SAMPLE_BUFFER SAMPLE_BUFFER;
typedef struct SOUND_SYSTEM SOUND_SYSTEM;
long S_CreateSoundSystem(SOUND_SYSTEM** out);
void S_DestroySoundSystem(SOUND_SYSTEM* sys);
long S_ConvertSamples(SOUND_SYSTEM* sys, unsigned char* data, long comp_size, long uncomp_size, long num, SAMPLE_BUFFER* buffers);
long S_StartSample(SOUND_SYSTEM* sys, SAMPLE_BUFFER* buffer, long volume, long pitch, long pan, unsigned long flags);
void S_SoundStopAllSamples(SOUND_SYSTEM* sys);
void S_SoundStopSample(SOUND_SYSTEM* sys, long num);
long S_SoundPlaySample(SOUND_SYSTEM* sys, SAMPLE_BUFFER* buffer, unsigned short volume, long pitch, short pan);
long S_SoundPlaySampleLooped(SOUND_SYSTEM* sys, SAMPLE_BUFFER* buffer, unsigned short volume, long pitch, short pan);
long S_SoundSampleIsPlaying(SOUND_SYSTEM* sys, long num);
void S_SoundSetPanAndVolume(SOUND_SYSTEM* sys, long num, short pan, unsigned short volume);
void S_SoundSetPitch(SOUND_SYSTEM* sys, long num, long pitch);
void S_SetReverbType(SOUND_SYSTEM* sys, long reverb);
SOUND_SYSTEM* soundImpl;
#endif// TOMB4_SPECIFIC_SOUND_H
