#ifndef TOMB4_SPECIFIC_AUDIO_H
#define TOMB4_SPECIFIC_AUDIO_H
void S_CDPlay(long track, long mode);
void S_CDStop();
void S_StartSyncedAudio(long track);
void S_CDClose();
extern long XATrack;
extern long XAFlag;


#endif// TOMB4_SPECIFIC_AUDIO_H
