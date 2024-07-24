#ifndef SPECIFIC_WINDOWS_DXSOUND_H
#define SPECIFIC_WINDOWS_DXSOUND_H
// clang-format off
#include <windows.h>
#include <mmiscapi.h>
#include <mmreg.h>
#include <MSAcm.h>
#include <dsound.h>
// clang-format on
typedef struct SAMPLE_BUFFER SAMPLE_BUFFER;
long DXChangeOutputFormat(long nSamplesPerSec, long force);
void DSChangeVolume(long num, long volume);
long DXDSCreate();
long DSIsChannelPlaying(long num);

void DXDSClose();

extern char* samples_buffer;
extern HACMDRIVER hACMDriver;
extern unsigned char* wav_file_buffer;
extern unsigned char* ADPCMBuffer;
extern long acm_ready;

#endif// SPECIFIC_WINDOWS_DXSOUND_H
