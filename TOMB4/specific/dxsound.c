
#include "specific/dxsound.h"
#include "specific/dxshell.h"
#include "specific/function_stubs.h"
#include "specific/audio.h"
#include "game/sound.h"
#include "specific/loadsave.h"
#include "specific/winmain.h"
#include "specific/dxinfo.h"
#include "specific/dxdirectsoundinfo.h"
#include <MSAcm.h>
#include <string.h>
#include <xaudio2.h>
#include <xaudio2fx.h>
#include "game/samplebuffer.h"
#include <xaudio2redist.h>
#include <dsound.h>
#pragma warning(push)
#pragma warning(disable : 4838)
#pragma warning(disable : 4309)
static unsigned char source_pcm_format[50] = {
	2, 0, 1, 0, 34, 86, 0, 0, 147, 43, 0, 0, 0, 2, 4, 0, 32, 0, 244, 3, 7, 0, 0, 1, 0, 0, 0, 2, 0, 255, 0, 0,
	0, 0, 192, 0, 64, 0, 240, 0, 0, 0, 204, 1, 48, 255, 136, 1, 24, 255
};
#pragma warning(pop)

static LPDIRECTSOUNDBUFFER DSPrimary;
static IXAudio2MasteringVoice* XAMaster;
static IUnknown* XAEffect;
static IXAudio2SourceVoice* XA_Voices[32];
static MMRESULT mmresult;
static WAVEFORMATEX pcm_format;


static XAUDIO2FX_REVERB_I3DL2_PARAMETERS reverb_preset[4] = {
	{ 50.0F, -1000, -500, 0.0F, 2.31F, 0.64F, -711, 0.012F, -800, 0.017F, 100.0F, 100.0F, 5000.0F }, // Small Room
	{ 50.0F, -1000, -500, 0.0F, 2.31F, 0.64F, -711, 0.012F, -300, 0.017F, 100.0F, 100.0F, 5000.0F }, // Medium Room
	{ 50.0F, -1000, -500, 0.0F, 2.31F, 0.64F, -711, 0.012F, 200, 0.017F, 100.0F, 100.0F, 5000.0F }, // Large Room
	{ 50.0F, -1000, -500, 0.0F, 2.31F, 0.64F, -711, 0.012F, 700, 0.017F, 100.0F, 100.0F, 5000.0F } // Pipe
};
static XAUDIO2FX_REVERB_PARAMETERS reverb_type[4];

static long current_reverb = -1;

bool DXChangeOutputFormat(long nSamplesPerSec, bool force) {
	WAVEFORMATEX pcfxFormat;
	static long lastSPC;

	if(!force && lastSPC == nSamplesPerSec)
		return 1;

	lastSPC = nSamplesPerSec;
	pcfxFormat.wFormatTag = WAVE_FORMAT_PCM;
	pcfxFormat.nChannels = 2;
	pcfxFormat.nSamplesPerSec = nSamplesPerSec;
	pcfxFormat.nAvgBytesPerSec = 4 * nSamplesPerSec;
	pcfxFormat.nBlockAlign = 4;
	pcfxFormat.wBitsPerSample = 16;
	pcfxFormat.cbSize = 0;
	S_SoundStopAllSamples();

	if(DSPrimary && DXAttempt(IDirectSoundBuffer_SetFormat(DSPrimary,&pcfxFormat)) != DS_OK) {
		Log(1, "Can't set sound output format to %d", pcfxFormat.nSamplesPerSec);
		return 0;
	}

	return 1;
}

void DSChangeVolume(long num, long volume) {
	float fvolume;

	if(XA_Voices[num]) {
		fvolume = XAudio2DecibelsToAmplitudeRatio(volume / 100.0F);
		IXAudio2SourceVoice_SetChannelVolumes(XA_Voices[num],1, &fvolume, XAUDIO2_COMMIT_NOW);
	}
}

void DSAdjustPitch(long num, long pitch) {
	unsigned long frequency;

	if(XA_Voices[num]) {
		frequency = (unsigned long)((float)pitch / 65536.0F * 22050.0F);

		if(frequency < 100)
			frequency = 100;
		else if(frequency > 100000)
			frequency = 100000;

		IXAudio2SourceVoice_SetFrequencyRatio(XA_Voices[num],frequency / 22050.0F, XAUDIO2_COMMIT_NOW);
	}
}

void DSAdjustPan(long num, long pan) {
	float matrix[2];

	if(XA_Voices[num]) {
		if(pan < 0) {
			if(pan < -0x4000)
				pan = -0x4000 - pan;
		} else if(pan > 0 && pan > 0x4000)
			pan = 0x8000 - pan;

		pan >>= 4;

		if(!pan) {
			matrix[0] = 1.0F;
			matrix[1] = 1.0F;
		} else if(pan < 0) {
			matrix[0] = 1.0F;
			matrix[1] = XAudio2DecibelsToAmplitudeRatio(pan / 100.0F);
		} else {
			matrix[0] = XAudio2DecibelsToAmplitudeRatio(-pan / 100.0F);
			matrix[1] = 1.0F;
		}

		IXAudio2SourceVoice_SetOutputMatrix(XA_Voices[num],0, 1, 2, matrix, XAUDIO2_COMMIT_NOW);
	}
}

bool DXSetOutputFormat() {
	DSBUFFERDESC desc;

	Log(2, "DXSetOutputFormat");
	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	if(DXAttempt(IDirectSound8_CreateSoundBuffer(App.dx.lpDS,&desc, &DSPrimary, 0)) == DS_OK) {
		DXChangeOutputFormat(sfx_frequencies[SoundQuality], 0);
		IDirectSoundBuffer_Play(DSPrimary,0, 0, DSBPLAY_LOOPING);
		return 1;
	}

	Log(1, "Can't Get Primary Sound Buffer");
	return 0;
}

bool DXDSCreate() {
	XAUDIO2_EFFECT_DESCRIPTOR chaind;
	XAUDIO2_EFFECT_CHAIN chain;

	Log(2, "DXDSCreate");
	pcm_format.wFormatTag = WAVE_FORMAT_PCM;
	pcm_format.cbSize = 0;
	pcm_format.nChannels = 1;
	pcm_format.nAvgBytesPerSec = 44100;
	pcm_format.nSamplesPerSec = 22050;
	pcm_format.nBlockAlign = 2;
	pcm_format.wBitsPerSample = 16;
	DXAttempt(DirectSoundCreate8(G_dxinfo->DSInfo[G_dxinfo->nDS].lpGuid, &App.dx.lpDS, 0));
	DXAttempt(IDirectSound8_SetCooperativeLevel(App.dx.lpDS,App.hWnd, DSSCL_EXCLUSIVE));
	DXSetOutputFormat();
	DXAttempt(XAudio2Create(&App.dx.lpXA, 0, XAUDIO2_DEFAULT_PROCESSOR));
	DXAttempt(XAudio2CreateReverb(&XAEffect, 0));
	chaind.pEffect = XAEffect;
	chaind.InitialState = TRUE;
	chaind.OutputChannels = 2;
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &chaind;
	DXAttempt(IXAudio2_CreateMasteringVoice(App.dx.lpXA,&XAMaster, 2, 44100, 0, 0, &chain, AudioCategory_GameEffects));

	for(int i = 0; i < 32; i++)
		DXAttempt(IXAudio2_CreateSourceVoice(App.dx.lpXA,&XA_Voices[i], &pcm_format, 0, XAUDIO2_MAX_FREQ_RATIO, 0, 0, 0));

	for(int i = 0; i < 4; i++)
		ReverbConvertI3DL2ToNative(&reverb_preset[i], &reverb_type[i], FALSE);

	sound_active = 1;
	return 1;
}

bool S_ConvertSamples(unsigned char* data, long comp_size, long uncomp_size, long num, SAMPLE_BUFFER* buffers) {
	HACMSTREAM hACMStream;
	ACMSTREAMHEADER ACMStreamHeader;
	mmresult = acmStreamOpen(&hACMStream, hACMDriver, (LPWAVEFORMATEX)source_pcm_format, &pcm_format, 0, 0, 0, 0);
	if(mmresult != DS_OK)
		Log(1, "Stream Open %d", mmresult);

	char* decompressed_samples_buffer = (char*)calloc(0xF0000,1);
	char* samples_buffer = (char*)calloc(0xF005A,1);
	memcpy(samples_buffer, data, comp_size);
	memset(&ACMStreamHeader, 0, sizeof(ACMStreamHeader));
	ACMStreamHeader.pbSrc = (unsigned char*)(samples_buffer + 90);
	ACMStreamHeader.cbStruct = 84;
	ACMStreamHeader.cbSrcLength = 0xF0000;
	ACMStreamHeader.cbDstLength = 0xF0000;
	ACMStreamHeader.pbDst = (unsigned char*)decompressed_samples_buffer;
	mmresult = acmStreamPrepareHeader(hACMStream, &ACMStreamHeader, 0);

	if(mmresult != DS_OK) {
		Log(1, "Prepare Stream %d", mmresult);
		acmStreamClose(hACMStream, 0);
		free(decompressed_samples_buffer);
		free(samples_buffer);
		return 0;
	}
	LPWAVEFORMATEX format;

	Log(8, "DXCreateSampleADPCM");

	if(!App.dx.lpDS) {
		acmStreamClose(hACMStream,0);
		free(decompressed_samples_buffer);
		free(samples_buffer);
		return 0;
	}

	format = (LPWAVEFORMATEX)(data + 20);

	if(format->nSamplesPerSec != 22050)
		Log(1, "Incorrect SamplesPerSec");

	ACMStreamHeader.cbSrcLength = comp_size - (sizeof(WAVEFORMATEX) + format->cbSize + 40);
	mmresult = acmStreamConvert(hACMStream, &ACMStreamHeader, ACM_STREAMCONVERTF_BLOCKALIGN | ACM_STREAMCONVERTF_START);

	if(mmresult != DS_OK)
		Log(1, "Stream Convert %d", mmresult);

	buffers[num].data = (BYTE*)calloc(uncomp_size,1);
	memcpy((void*)buffers[num].data, decompressed_samples_buffer, uncomp_size - 32);
	buffers[num].dataSize = uncomp_size - 32;
	ACMStreamHeader.cbSrcLength = 0xF0000;
	mmresult = acmStreamUnprepareHeader(hACMStream, &ACMStreamHeader, 0);

	if(mmresult != DS_OK) {
		Log(1, "UnPrepare Stream %d", mmresult);
	}

	mmresult = acmStreamClose(hACMStream, 0);
	if(mmresult != DS_OK) {
		Log(1, "Stream Close %d", mmresult);
	}

	free(decompressed_samples_buffer);
	free(samples_buffer);
	return 1;
}

void DXStopSample(long num) {
	if(num >= 0 && XA_Voices[num]) {
		DXAttempt(IXAudio2SourceVoice_Stop(XA_Voices[num],0, XAUDIO2_COMMIT_NOW));
		DXAttempt(IXAudio2SourceVoice_FlushSourceBuffers(XA_Voices[num]));
	}
}

bool DSIsChannelPlaying(long num) {
	XAUDIO2_VOICE_STATE state;

	if(XA_Voices[num]) {
		IXAudio2SourceVoice_GetState(XA_Voices[num],&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);

		if(state.BuffersQueued)
			return 1;
	}

	return 0;
}

long DSGetFreeChannel() {
	for(int i = 0; i < 32; i++) {
		if(!DSIsChannelPlaying(i))
			return i;
	}

	return -1;
}

long S_StartSample(SAMPLE_BUFFER* data, long volume, long pitch, long pan, unsigned long flags) {
	IXAudio2SourceVoice* voice;
	XAUDIO2_BUFFER* buffer;
	long channel;

	channel = DSGetFreeChannel();

	if(channel < 0)
		return -1;

	voice = XA_Voices[channel];
	DSChangeVolume(channel, volume);
	DSAdjustPitch(channel, pitch);
	DSAdjustPan(channel, pan);
	XAUDIO2_BUFFER buf;
	memset(&buf,0,sizeof(buf));
	buf.AudioBytes = data->dataSize;
	buf.pAudioData = data->data;
	buffer = &buf;
	buffer->LoopCount = flags;
	DXAttempt(IXAudio2SourceVoice_SubmitSourceBuffer(voice,buffer, 0));
	DXAttempt(IXAudio2SourceVoice_Start(voice,0, XAUDIO2_COMMIT_NOW));
	return channel;
}

long CalcVolume(long volume) {
	long result;

	result = 8000 - (long)((float)(0x7FFF - volume) * 0.30518511F);

	if(result > 0)
		result = 0;
	else if(result < -10000)
		result = -10000;

	result -= (100 - SFXVolume) * 50;

	if(result > 0)
		result = 0;

	if(result < -10000)
		result = -10000;

	return result;
}

void S_SoundStopAllSamples() {
	for(int i = 0; i < 32; i++)
		DXStopSample(i);
}

void S_SoundStopSample(long num) {
	DXStopSample(num);
}

long S_SoundPlaySample(SAMPLE_BUFFER* buffer, unsigned short volume, long pitch, short pan) {
	return S_StartSample(buffer, CalcVolume(volume), pitch, pan, 0);
}

long S_SoundPlaySampleLooped(SAMPLE_BUFFER* buffer, unsigned short volume, long pitch, short pan) {
	return S_StartSample(buffer, CalcVolume(volume), pitch, pan, XAUDIO2_LOOP_INFINITE);
}

void DXFreeSounds() {
	S_SoundStopAllSamples();
}

long S_SoundSampleIsPlaying(long num) {
	if(sound_active && DSIsChannelPlaying(num))
		return 1;

	return 0;
}

void S_SoundSetPanAndVolume(long num, short pan, unsigned short volume) {
	if(sound_active) {
		DSChangeVolume(num, CalcVolume(volume));
		DSAdjustPan(num, pan);
	}
}

void S_SoundSetPitch(long num, long pitch) {
	if(sound_active)
		DSAdjustPitch(num, pitch);
}

void S_SetReverbType(long reverb) {
	if(App.SoundDisabled)
		return;

	if(/*tomb4.reverb == 1*/ true)
		reverb = 0;

	if(current_reverb != reverb) {
		if(reverb) {
			if(!current_reverb) {
				IXAudio2MasteringVoice_EnableEffect(XAMaster,0, XAUDIO2_COMMIT_NOW);
				IXAudio2MasteringVoice_SetVolume(XAMaster,2.0F, XAUDIO2_COMMIT_NOW);
			}

			IXAudio2MasteringVoice_SetEffectParameters(XAMaster,0, &reverb_type[reverb - 1], sizeof(XAUDIO2FX_REVERB_PARAMETERS), XAUDIO2_COMMIT_NOW);
		} else {
			IXAudio2MasteringVoice_DisableEffect(XAMaster,0, XAUDIO2_COMMIT_NOW);
			IXAudio2MasteringVoice_SetVolume(XAMaster,1.0F, XAUDIO2_COMMIT_NOW);
		}

		current_reverb = reverb;
	}
}

void DXDSClose() {
	if(App.SoundDisabled)
		return;

	for(int i = 0; i < 32; i++) {
		if(XA_Voices[i]) {
			IXAudio2SourceVoice_DestroyVoice(XA_Voices[i]);
			XA_Voices[i] = 0;
		}
	}

	IXAudio2MasteringVoice_DestroyVoice(XAMaster);
	IUnknown_Release(XAEffect);
	IUnknown_Release(App.dx.lpXA);
}
