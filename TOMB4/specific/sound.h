#ifndef TOMB4_SPECIFIC_SOUND_H
#define TOMB4_SPECIFIC_SOUND_H
typedef struct SAMPLE_BUFFER SAMPLE_BUFFER;
typedef struct SOUND_SYSTEM SOUND_SYSTEM;
int S_CreateSoundSystem(SOUND_SYSTEM** out);
void S_DestroySoundSystem(SOUND_SYSTEM* sys);
int S_ConvertSamples(SOUND_SYSTEM* sys, unsigned char* data, int comp_size, int uncomp_size, int num, SAMPLE_BUFFER* buffers);
int S_StartSample(SOUND_SYSTEM* sys, SAMPLE_BUFFER* buffer, int volume, int pitch, int pan, unsigned int flags);
void S_SoundStopAllSamples(SOUND_SYSTEM* sys);
void S_SoundStopSample(SOUND_SYSTEM* sys, int num);
int S_SoundPlaySample(SOUND_SYSTEM* sys, SAMPLE_BUFFER* buffer, unsigned short volume, int pitch, short pan);
int S_SoundPlaySampleLooped(SOUND_SYSTEM* sys, SAMPLE_BUFFER* buffer, unsigned short volume, int pitch, short pan);
int S_SoundSampleIsPlaying(SOUND_SYSTEM* sys, int num);
void S_SoundSetPanAndVolume(SOUND_SYSTEM* sys, int num, short pan, unsigned short volume);
void S_SoundSetPitch(SOUND_SYSTEM* sys, int num, int pitch);
void S_SetReverbType(SOUND_SYSTEM* sys, int reverb);
void S_SetSoundVolume(SOUND_SYSTEM* sys, int volume);
void S_StopSampleLoop(SOUND_SYSTEM* sys, int num);
SOUND_SYSTEM* soundImpl;
#endif// TOMB4_SPECIFIC_SOUND_H
