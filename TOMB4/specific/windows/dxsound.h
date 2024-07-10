#ifndef SPECIFIC_WINDOWS_DXSOUND_H
#define SPECIFIC_WINDOWS_DXSOUND_H
#include <stdbool.h> 
typedef struct SAMPLE_BUFFER SAMPLE_BUFFER;
bool DXChangeOutputFormat(long nSamplesPerSec, bool force);
void DSChangeVolume(long num, long volume);
bool DXDSCreate();
bool DSIsChannelPlaying(long num);

void DXDSClose();

extern char* samples_buffer;


#endif// SPECIFIC_WINDOWS_DXSOUND_H
