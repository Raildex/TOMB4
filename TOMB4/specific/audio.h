#ifndef TOMB4_SPECIFIC_AUDIO_H
#define TOMB4_SPECIFIC_AUDIO_H
typedef struct MUSIC_SYSTEM MUSIC_SYSTEM;

typedef enum track_modes {
	TRACK_MODE_ATMOSPHERE,
	TRACK_MODE_INCIDENT,
	TRACK_MODE_INCIDENT_RESTORE_ATMOSPHERE
} track_modes;

long S_CreateMusicSystem(MUSIC_SYSTEM** out);
void S_DestroyMusicSystem(MUSIC_SYSTEM* sys);

void S_PlayTrack(MUSIC_SYSTEM* system, long track, track_modes mode);
void S_StopTrack(MUSIC_SYSTEM* system);
void S_PlaySyncedTrack(MUSIC_SYSTEM* system, long track);
void S_DestroyMusicSystem(MUSIC_SYSTEM* sys);
void S_ApplyMusicVolume(MUSIC_SYSTEM* sys, long volume);
extern MUSIC_SYSTEM* musicImpl;

#endif// TOMB4_SPECIFIC_AUDIO_H
