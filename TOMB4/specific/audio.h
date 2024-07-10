#ifndef TOMB4_SPECIFIC_AUDIO_H
#define TOMB4_SPECIFIC_AUDIO_H
#include <windows.h>
#include <mmreg.h>
#include <mmiscapi.h>
#include <MSAcm.h>
#include <stdbool.h>
void OpenStreamFile(char* name);
void GetADPCMData();
void ACMSetVolume();
void ACMEmulateCDPlay(long track, long mode);
BOOL WINAPI ACMEnumCallBack(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport);
long ACMSetupNotifications();
void FillADPCMBuffer(char* p, long track);
long ACMHandleNotifications();
bool ACMInit();
void ACMClose();
void S_CDPlay(long track, long mode);
void S_CDStop();
void S_StartSyncedAudio(long track);

extern HACMDRIVER hACMDriver;
extern unsigned char* wav_file_buffer;
extern unsigned char* ADPCMBuffer;
extern bool acm_ready;

extern long XATrack;
extern long XAFlag;


#endif// TOMB4_SPECIFIC_AUDIO_H
