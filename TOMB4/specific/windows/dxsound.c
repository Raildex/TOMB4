#include "game/samplebuffer.h"
#include "game/sound.h"
#include "specific/function_stubs.h"
#include "specific/loadsave.h"
#include "specific/sound.h"
#include "specific/windows/dxshell.h"
#include "specific/windows/winmain.h"
#include "xaudio2fxredist.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winbase.h>
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <xaudio2redist.h>

// header order important here
// clang-format off
#include <windows.h>
#include <unknwnbase.h>
#include <winnt.h>
#include <mmreg.h>
#include <mmiscapi.h>
#include <MSAcm.h>
// clang-format on
#include <dsound.h>

typedef struct SOUND_SYSTEM {
	IXAudio2* x2;
	IXAudio2MasteringVoice* master;
	IXAudio2SourceVoice* sourceVoices[32];
	IUnknown* reverb;
	WAVEFORMATEX* adpcmfmt;
	WAVEFORMATEX pcmfmt;
	long reverbType;
	XAUDIO2FX_REVERB_PARAMETERS* reverb_types;
} SOUND_SYSTEM;

long S_ConvertSamples(SOUND_SYSTEM* sys, unsigned char* data, long comp_size, long uncomp_size, long num, SAMPLE_BUFFER* buffers) {
	HACMSTREAM hACMStream;

	MMRESULT mmresult = acmStreamOpen(&hACMStream, NULL, sys->adpcmfmt, &sys->pcmfmt, 0, 0, 0, 0);
	if(mmresult != DS_OK) {
		Log(__func__, "Stream Open %d", mmresult);
	}

	char* decompressed_samples_buffer = (char*)calloc(0xF0000, 1);
	char* samples_buffer = (char*)calloc(0xF005A, 1);
	memcpy(samples_buffer, data, comp_size);
	ACMSTREAMHEADER ACMStreamHeader = { 0 };
	ACMStreamHeader.pbSrc = (unsigned char*)(samples_buffer + 90);
	ACMStreamHeader.cbStruct = 84;
	ACMStreamHeader.cbSrcLength = 0xF0000;
	ACMStreamHeader.cbDstLength = 0xF0000;
	ACMStreamHeader.pbDst = (unsigned char*)decompressed_samples_buffer;
	mmresult = acmStreamPrepareHeader(hACMStream, &ACMStreamHeader, 0);

	if(mmresult != DS_OK) {
		Log(__func__, "Prepare Stream %d", mmresult);
		acmStreamClose(hACMStream, 0);
		free(decompressed_samples_buffer);
		free(samples_buffer);
		return 0;
	}
	LPWAVEFORMATEX format;

	Log(__func__, "DXCreateSampleADPCM");

	format = (LPWAVEFORMATEX)(data + 20);

	if(format->nSamplesPerSec != 22050) {
		Log(__func__, "Incorrect SamplesPerSec");
	}

	ACMStreamHeader.cbSrcLength = comp_size - (sizeof(WAVEFORMATEX) + format->cbSize + 40);
	mmresult = acmStreamConvert(hACMStream, &ACMStreamHeader, ACM_STREAMCONVERTF_BLOCKALIGN | ACM_STREAMCONVERTF_START);

	if(mmresult != DS_OK) {
		Log(__func__, "Stream Convert %d", mmresult);
	}

	buffers[num].data = (BYTE*)calloc(uncomp_size, 1);
	memcpy((void*)buffers[num].data, decompressed_samples_buffer, uncomp_size - 32);
	buffers[num].dataSize = uncomp_size - 32;
	ACMStreamHeader.cbSrcLength = 0xF0000;
	mmresult = acmStreamUnprepareHeader(hACMStream, &ACMStreamHeader, 0);

	if(mmresult != DS_OK) {
		Log(__func__, "UnPrepare Stream %d", mmresult);
	}

	mmresult = acmStreamClose(hACMStream, 0);
	if(mmresult != DS_OK) {
		Log(__func__, "Stream Close %d", mmresult);
	}

	free(decompressed_samples_buffer);
	free(samples_buffer);
	return 1;
}

void StopSample(SOUND_SYSTEM* sys, long num) {
	if(num >= 0 && sys->sourceVoices[num]) {
		DXAttempt(IXAudio2SourceVoice_Stop(sys->sourceVoices[num], 0, XAUDIO2_COMMIT_NOW));
		DXAttempt(IXAudio2SourceVoice_FlushSourceBuffers(sys->sourceVoices[num]));
	}
}

long IsChannelPlaying(SOUND_SYSTEM* sys, long num) {
	XAUDIO2_VOICE_STATE state;

	if(sys->sourceVoices[num]) {
		IXAudio2SourceVoice_GetState(sys->sourceVoices[num], &state, XAUDIO2_VOICE_NOSAMPLESPLAYED);

		if(state.BuffersQueued) {
			return 1;
		}
	}

	return 0;
}

long GetFreeChannel(SOUND_SYSTEM* sys) {
	for(int i = 0; i < 32; i++) {
		if(!IsChannelPlaying(sys, i)) {
			return i;
		}
	}
	return -1;
}

long S_StartSample(SOUND_SYSTEM* sys, SAMPLE_BUFFER* data, long volume, long pitch, long pan, unsigned long flags) {
	IXAudio2SourceVoice* voice;
	XAUDIO2_BUFFER* buffer;
	long channel;

	channel = GetFreeChannel(sys);

	if(channel < 0) {
		return -1;
	}

	voice = sys->sourceVoices[channel];

	XAUDIO2_BUFFER buf = { 0 };
	buf.AudioBytes = data->dataSize;
	buf.pAudioData = data->data;
	buffer = &buf;
	buffer->LoopCount = flags;
	DXAttempt(IXAudio2SourceVoice_SubmitSourceBuffer(voice, buffer, XAUDIO2_COMMIT_NOW));
	DXAttempt(IXAudio2SourceVoice_Start(voice, 0, XAUDIO2_COMMIT_NOW));
	S_SoundSetPanAndVolume(sys, channel, pan, volume);
	S_SoundSetPitch(sys, channel, pitch);
	return channel;
}

long CalcVolume(long volume) {
	long result;

	result = 8000 - (long)((float)(0x7FFF - volume) * 0.30518511F);

	if(result > 0) {
		result = 0;
	} else if(result < -10000) {
		result = -10000;
	}

	result -= (100 - SFXVolume) * 50;

	if(result > 0) {
		result = 0;
	}

	if(result < -10000) {
		result = -10000;
	}

	return result;
}

void S_SoundStopAllSamples(SOUND_SYSTEM* sys) {
	for(int i = 0; i < 32; i++) {
		StopSample(sys, i);
	}
}

void S_SoundStopSample(SOUND_SYSTEM* sys, long num) {
	StopSample(sys, num);
}

long S_SoundPlaySample(SOUND_SYSTEM* sys, SAMPLE_BUFFER* buffer, unsigned short volume, long pitch, short pan) {
	return S_StartSample(sys, buffer, CalcVolume(volume), pitch, pan, 0);
}

long S_SoundPlaySampleLooped(SOUND_SYSTEM* sys, SAMPLE_BUFFER* buffer, unsigned short volume, long pitch, short pan) {
	return S_StartSample(sys, buffer, CalcVolume(volume), pitch, pan, XAUDIO2_LOOP_INFINITE);
}

long S_SoundSampleIsPlaying(SOUND_SYSTEM* sys, long num) {
	if(num < 0) {
		return 0;
	}
	if(sound_active && IsChannelPlaying(sys, num)) {
		return 1;
	}

	return 0;
}

long S_SoundSetVolume(SOUND_SYSTEM* sys, long num, long volume) {
	float fvolume = XAudio2DecibelsToAmplitudeRatio(volume / 65535.0F);
	IXAudio2SourceVoice_SetChannelVolumes(sys->sourceVoices[num], 1, &fvolume, XAUDIO2_COMMIT_NOW);
	return 1;
}

void S_SoundSetPanAndVolume(SOUND_SYSTEM* sys, long num, short pan, unsigned short volume) {
	S_SoundSetVolume(sys, num, volume);
	float matrix[2];
	if(pan < 0) {
		if(pan < -0x4000) {
			pan = -0x4000 - pan;
		}
	} else if(pan > 0 && pan > 0x4000) {
		pan = 0x8000 - pan;
	}

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
	IXAudio2Voice_SetOutputMatrix(sys->sourceVoices[num], NULL, 1, 2, matrix, XAUDIO2_COMMIT_NOW);
}

void S_SoundSetPitch(SOUND_SYSTEM* sys, long num, long pitch) {
	float frequency = ((float)pitch / 65536.0F);
	IXAudio2SourceVoice_SetFrequencyRatio(sys->sourceVoices[num], frequency, XAUDIO2_COMMIT_NOW);
}

void S_SetReverbType(SOUND_SYSTEM* sys, long reverb) {
	if(App.SoundDisabled) {
		return;
	}

	if(sys->reverbType != reverb) {
		if(reverb) {
			if(!sys->reverbType) {
				IXAudio2MasteringVoice_EnableEffect(sys->master, 0, XAUDIO2_COMMIT_NOW);
				IXAudio2MasteringVoice_SetVolume(sys->master, 2.0F, XAUDIO2_COMMIT_NOW);
			}

			IXAudio2MasteringVoice_SetEffectParameters(sys->master, 0, &sys->reverb_types[reverb - 1], sizeof(XAUDIO2FX_REVERB_PARAMETERS), XAUDIO2_COMMIT_NOW);
		} else {
			IXAudio2MasteringVoice_DisableEffect(sys->master, 0, XAUDIO2_COMMIT_NOW);
			IXAudio2MasteringVoice_SetVolume(sys->master, 1.0F, XAUDIO2_COMMIT_NOW);
		}

		sys->reverbType = reverb;
	}
}

static long Check(const char* scope, HRESULT res) {
	if(FAILED(res)) {
		char buffer[256];
		long n = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, res, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), buffer, sizeof(buffer), NULL);
		LogE(scope, "DirectInput Error: %.*s", n, buffer);
		return 0;
	}
	return 1;
}

WAVEFORMATEX* CreateADPCMFormat() {
	ADPCMWAVEFORMAT* adpcmfmt = calloc(1, sizeof(ADPCMWAVEFORMAT) + sizeof(ADPCMCOEFSET) * 7); // MS-ADPCM
	if(!adpcmfmt) {
		return NULL;
	}
	adpcmfmt->wfx.nSamplesPerSec = 22050;
	adpcmfmt->wfx.nAvgBytesPerSec = 11155;
	adpcmfmt->wfx.nBlockAlign = 512;
	adpcmfmt->wfx.wBitsPerSample = 4;
	adpcmfmt->wfx.cbSize = 32;
	adpcmfmt->wfx.wFormatTag = WAVE_FORMAT_ADPCM;
	adpcmfmt->wfx.nChannels = 1;
	adpcmfmt->wNumCoef = 7;
	adpcmfmt->wSamplesPerBlock = adpcmfmt->wfx.nBlockAlign * 2 / adpcmfmt->wfx.nChannels - 12;
	adpcmfmt->aCoef[0].iCoef1 = 256;
	adpcmfmt->aCoef[0].iCoef2 = 0;
	adpcmfmt->aCoef[1].iCoef1 = 512;
	adpcmfmt->aCoef[1].iCoef2 = -256;
	adpcmfmt->aCoef[2].iCoef1 = 0;
	adpcmfmt->aCoef[2].iCoef2 = 0;
	adpcmfmt->aCoef[3].iCoef1 = 192;
	adpcmfmt->aCoef[3].iCoef2 = 64;
	adpcmfmt->aCoef[4].iCoef1 = 240;
	adpcmfmt->aCoef[4].iCoef2 = 0;
	adpcmfmt->aCoef[5].iCoef1 = 460;
	adpcmfmt->aCoef[5].iCoef2 = -208;
	adpcmfmt->aCoef[6].iCoef1 = 392;
	adpcmfmt->aCoef[6].iCoef2 = -232;
	return (WAVEFORMATEX*)adpcmfmt;
}

long S_CreateSoundSystem(SOUND_SYSTEM** out) {
	SOUND_SYSTEM* system = (SOUND_SYSTEM*)calloc(1, sizeof(SOUND_SYSTEM));
	if(!system) {
		return 0;
	}
	system->reverbType = -1;
	XAUDIO2_DEBUG_CONFIGURATION config = { 0 };
	config.LogFunctionName = 1;
	config.LogFileline = 1;
	config.BreakMask = XAUDIO2_LOG_ERRORS;
	config.TraceMask = XAUDIO2_LOG_ERRORS;
	if(!Check(__func__, XAudio2Create(&system->x2, XAUDIO2_DEBUG_ENGINE, Processor1))) {
		free(system);
		return 0;
	}
	IXAudio2_SetDebugConfiguration(system->x2, &config, NULL);
	if(!Check(__func__, XAudio2CreateReverb(&system->reverb, 0))) {
		IUnknown_Release(system->x2);
		free(system);
		return 0;
	}
	XAUDIO2_EFFECT_DESCRIPTOR reverb = { 0 };
	reverb.pEffect = system->reverb;
	reverb.OutputChannels = 2;
	reverb.InitialState = 1;
	XAUDIO2_EFFECT_CHAIN chain = { 0 };
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &reverb;
	if(!Check(__func__, IXAudio2_CreateMasteringVoice(system->x2, &system->master, 2, 44100, 0, NULL, &chain, AudioCategory_SoundEffects))) {
		IUnknown_Release(system->reverb);
		IUnknown_Release(system->x2);
		free(system);
		return 0;
	}
	system->pcmfmt.wFormatTag = WAVE_FORMAT_PCM;
	system->pcmfmt.nAvgBytesPerSec = 22050 * 2;
	system->pcmfmt.nChannels = 1;
	system->pcmfmt.wBitsPerSample = 16;
	system->pcmfmt.nBlockAlign = 2;
	system->pcmfmt.nSamplesPerSec = 22050;

	system->adpcmfmt = CreateADPCMFormat();
	if(!system->adpcmfmt) {
		IUnknown_Release(system->reverb);
		IUnknown_Release(system->x2);
		free(system);
		return 0;
	}

	for(int i = 0; i < 32; ++i) {
		if(!Check(__func__, IXAudio2_CreateSourceVoice(system->x2, &system->sourceVoices[i], &system->pcmfmt, XAUDIO2_VOICE_USEFILTER, 10.0f, NULL, NULL, NULL))) {
			IUnknown_Release(system->reverb);
			IUnknown_Release(system->x2);
			free(system);
			return 0;
		}
	}
	XAUDIO2FX_REVERB_I3DL2_PARAMETERS reverb_presets[5] = {
		(XAUDIO2FX_REVERB_I3DL2_PARAMETERS){ 50.0F, -1000, -500, 0.0F, 2.31F, 0.64F, -711, 0.012F, -800, 0.017F, 100.0F, 100.0F, 5000.0F },
		(XAUDIO2FX_REVERB_I3DL2_PARAMETERS){ 50.0F, -1000, -500, 0.0F, 2.31F, 0.64F, -711, 0.012F, -300, 0.017F, 100.0F, 100.0F, 5000.0F },
		(XAUDIO2FX_REVERB_I3DL2_PARAMETERS){ 50.0F, -1000, -500, 0.0F, 2.31F, 0.64F, -711, 0.012F, 200, 0.017F, 100.0F, 100.0F, 5000.0F },
		(XAUDIO2FX_REVERB_I3DL2_PARAMETERS){ 50.0F, -1000, -500, 0.0F, 2.31F, 0.64F, -711, 0.012F, 700, 0.017F, 100.0F, 100.0F, 5000.0F },
		(XAUDIO2FX_REVERB_I3DL2_PARAMETERS){ 100, -200, -500, 0.0f, 0.49f, 0.10f, 0, 0.002f, 0, 0.0f, 100.0f, 100.0f, 1000.0f },
	};
	system->reverb_types = (XAUDIO2FX_REVERB_PARAMETERS*)calloc(16, sizeof(XAUDIO2FX_REVERB_PARAMETERS));
	if(!system->reverb_types) {
		IUnknown_Release(system->reverb);
		IUnknown_Release(system->x2);
		free(system);
		return 0;
	}
	for(int i = 0; i < 5; ++i) {
		ReverbConvertI3DL2ToNative(&reverb_presets[i], &system->reverb_types[i], 0);
		if(i == 4) {
			system->reverb_types[i].WetDryMix = 100;
			system->reverb_types[i].RoomFilterFreq = 200;
			system->reverb_types[i].DisableLateField = 1;
		}
	}
	*out = system;
	return 1;
}

void S_DestroySoundSystem(SOUND_SYSTEM* sys) {
	for(int i = 0; i < 32; ++i) {
		IXAudio2Voice_DestroyVoice(sys->sourceVoices[i]);
	}
	IXAudio2Voice_DestroyVoice(sys->master);
	IUnknown_Release(sys->reverb);
	IUnknown_Release(sys->x2);
	free(sys->adpcmfmt);
	free(sys->reverb_types);
	free(sys);
}

void S_SetSoundVolume(SOUND_SYSTEM* sys, long volume) {
	float v = volume / 100.0F;
	IXAudio2MasteringVoice_SetVolume(sys->master,v,XAUDIO2_COMMIT_NOW);
}

void S_StopSampleLoop(SOUND_SYSTEM* sys, long num) {
	IXAudio2SourceVoice_ExitLoop(sys->sourceVoices[num], XAUDIO2_COMMIT_NOW);
}