
#include "specific/audio.h"
#include "specific/function_stubs.h"
#include "specific/windows/dxptr.h"
#include "specific/windows/winmain.h"
#include "xaudio2redist.h"
#include <AudioSessionTypes.h>
#include <assert.h>
#include <fileapi.h>
#include <handleapi.h>
#include <minwinbase.h>
#include <processenv.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <unknwnbase.h>
#include <winbase.h>
// clang-format off
#include <windows.h>
#include <mmiscapi.h>
#include <mmreg.h>
#include <MSAcm.h>
#include <winnt.h>
#include <dsound.h>
// clang-format on
#define MINIAUDIO_IMPLEMENTATION
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_ENABLE_WASAPI
#define MA_NO_ENCODING
#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_NODE_GRAPH
#define MA_NO_ENGINE
#define MA_NO_THREADING
#define MA_NO_GENERATION
#define MA_NO_RUNTIME_LINKING
#define MA_DEBUG_OUTPUT
#include "miniaudio.h"

typedef struct STREAM_PLAYER_DATA STREAM_PLAYER_DATA;
long DecodeFrame(STREAM_PLAYER_DATA* player);

enum {
	STREAM_NEEDS_MORE_DATA,
	STREAM_STOP,
	STREAM_PLAYING,
};

typedef struct STREAM_PLAYER_DATA {
	char bufferedData[3][1024 * 85]; // 256KiB in total
	char currentWrittenBuffer;
	MUSIC_SYSTEM* sys;
	_Atomic(long) state;
	thrd_t decoderThread;
	IXAudio2SourceVoice* audioVoice;
	IXAudio2VoiceCallback callback;
	ma_decoder minidecoder;
	mtx_t notifyLock;
	cnd_t wakeCondition;
} STREAM_PLAYER_DATA;

typedef struct MUSIC_SYSTEM {
	IXAudio2* x2;
	IXAudio2MasteringVoice* master;
	STREAM_PLAYER_DATA* player;
	IXAudio2VoiceCallback callback;
	IXAudio2VoiceCallbackVtbl callbackFuncs;
	char memorizedAtmosphere[256];
	size_t memorizedFrame;
} MUSIC_SYSTEM;

static long Check(const char* scope, HRESULT result) {
	if(FAILED(result)) {
		char buffer[256];
		long n = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), buffer, sizeof(buffer), NULL);
		LogE(scope, "XAudio2 Error: %.*s", n, buffer);
		return 0;
	}
	return 1;
}

void DestroyPlayer(MUSIC_SYSTEM* sys) {
	if(!sys->player) {
		return;
	}
	atomic_store(&sys->player->state, STREAM_STOP);
	cnd_signal(&sys->player->wakeCondition);
	int res;
	int joined = thrd_join(sys->player->decoderThread, &res);
	assert(joined == thrd_success);
	IXAudio2Voice_DestroyVoice(sys->player->audioVoice);
	cnd_destroy(&sys->player->wakeCondition);
	mtx_destroy(&sys->player->notifyLock);
	ma_decoder_uninit(&sys->player->minidecoder);
	free(sys->player);
	sys->player = NULL;
}

int DecodeThreadFunc(void* arg) {
	STREAM_PLAYER_DATA* player = (STREAM_PLAYER_DATA*)arg;
	do {
		long state = atomic_load(&player->state);
		if(state == STREAM_NEEDS_MORE_DATA) {
			DecodeFrame(player);
		} else if(state == STREAM_STOP) {
			break;
		}
		mtx_lock(&player->notifyLock);
		cnd_wait(&player->wakeCondition, &player->notifyLock);
		mtx_unlock(&player->notifyLock);
	} while(1);
	return 0;
}

void WINAPI StreamEnded(IXAudio2VoiceCallback* cb) {
}

void WINAPI BufferStarted(IXAudio2VoiceCallback* cb, void* ctx) {
	STREAM_PLAYER_DATA* player = (STREAM_PLAYER_DATA*)ctx;
	long current = atomic_load(&player->state);
	if(current == STREAM_STOP) {
		cnd_signal(&player->wakeCondition);
		return;
	}
	atomic_store(&player->state, STREAM_NEEDS_MORE_DATA);
	cnd_signal(&player->wakeCondition);
}

void WINAPI LoopEnded(IXAudio2VoiceCallback* cb, void* ctx) {
}

void WINAPI VoiceError(IXAudio2VoiceCallback* cb, void* context, HRESULT err) {
	LogE(__func__, "Voice Error: %d", err);
}

void WINAPI VoiceProcessStarted(IXAudio2VoiceCallback* cb, UINT32 requiredBytes) {
}

void WINAPI VoiceProcessEnded(IXAudio2VoiceCallback* cb) {
}

void WINAPI BufferEnded(IXAudio2VoiceCallback* cb, void* context) {

}

long CreatePlayer(MUSIC_SYSTEM* sys, STREAM_PLAYER_DATA** out, const char* filepath) {
	STREAM_PLAYER_DATA* player = calloc(1, sizeof(STREAM_PLAYER_DATA));
	if(!player) {
		return 0;
	}
	ma_decoder_config cfg =  ma_decoder_config_init_default();
	cfg.ditherMode = ma_dither_mode_triangle;
	cfg.format = ma_format_f32;
	if(ma_decoder_init_file(filepath, &cfg, &player->minidecoder) != MA_SUCCESS) {
		free(player);
		return 0;
	}

	WAVEFORMATEX fmt = { 0 };
	fmt.cbSize = 0;
	fmt.nSamplesPerSec = player->minidecoder.outputSampleRate;
	fmt.wBitsPerSample = ma_get_bytes_per_sample(player->minidecoder.outputFormat) * 8;
	fmt.nChannels = player->minidecoder.outputChannels;
	fmt.nBlockAlign = ma_get_bytes_per_frame(player->minidecoder.outputFormat,player->minidecoder.outputChannels);
	if(player->minidecoder.outputFormat != ma_format_f32) {
		fmt.wFormatTag = WAVE_FORMAT_PCM;
	}else {
		fmt.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	}
	
	fmt.nAvgBytesPerSec = (fmt.nSamplesPerSec * fmt.wBitsPerSample * fmt.nChannels) / 8;

	if(!Check(__func__, IXAudio2_CreateSourceVoice(sys->x2, &player->audioVoice, &fmt, 0, XAUDIO2_MAX_FREQ_RATIO, &sys->callback, NULL, NULL))) {
		ma_decoder_uninit(&player->minidecoder);
		free(player);
		return 0;
	}
	if(!Check(__func__, IXAudio2SourceVoice_Start(player->audioVoice, 0, XAUDIO2_COMMIT_NOW))) {
		IXAudio2Voice_DestroyVoice(player->audioVoice);
		ma_decoder_uninit(&player->minidecoder);
		free(player);
		return 0;
	}

	atomic_init(&player->state, STREAM_NEEDS_MORE_DATA);
	player->sys = sys;
	if(thrd_create(&player->decoderThread, DecodeThreadFunc, player) != thrd_success) {
		ma_decoder_uninit(&player->minidecoder);
		IXAudio2Voice_DestroyVoice(player->audioVoice);
		free(player);
		return 0;
	}
	*out = player;
	return 1;
}

void S_PlayTrack(MUSIC_SYSTEM* sys, long track, track_modes mode) {
	if(sys->player) {
		if(mode == TRACK_MODE_INCIDENT_RESTORE_ATMOSPHERE) {
			ma_int64 cursor;
			ma_decoder_tell_bytes(&sys->player->minidecoder, &cursor);
			sys->memorizedFrame = cursor;
		}else {
			sys->memorizedFrame = 0;
		}
		DestroyPlayer(sys);
	}
	WIN32_FIND_DATAA findData;
	HANDLE hFind;
	char buffer[32];
	snprintf(buffer, 32, "audio/%03ld*",track);
	hFind = FindFirstFileA(buffer,&findData);
	if(hFind == INVALID_HANDLE_VALUE) {
		LogE(__func__, "Could not find track with number %ld", track);
		return;
	}
	char dir[256];
	long pathSize = GetCurrentDirectoryA(256,dir);
	char fullPath[256];
	snprintf(fullPath, 256, "%s/audio/%s",dir,findData.cFileName);
	if(!CreatePlayer(sys, &sys->player, fullPath)) {
		LogE(__func__, "Could not create audio player!");
		FindClose(hFind);
	}
	FindClose(hFind);
	atomic_store(&sys->player->state, STREAM_NEEDS_MORE_DATA);
	if(mode == TRACK_MODE_ATMOSPHERE) {
		memcpy(&sys->memorizedAtmosphere[0],&fullPath[0],256);
	}
}

void S_StopTrack(MUSIC_SYSTEM* system) {
	DestroyPlayer(system);
	
}

void S_PlaySyncedTrack(MUSIC_SYSTEM* system, long track) {
	S_StopTrack(system);
	S_PlayTrack(system, track, 2);
}

void S_DestroyMusicSystem(MUSIC_SYSTEM* sys) {
}

long DecodeFrame(STREAM_PLAYER_DATA* player) {
	char* destination = &player->bufferedData[player->currentWrittenBuffer][0];
	player->currentWrittenBuffer = (player->currentWrittenBuffer + 1) % 3; // looping through buffers
	ma_uint64 frameSize = ma_get_bytes_per_sample(player->minidecoder.outputFormat) * player->minidecoder.outputChannels;
	ma_uint64 framesToRead = sizeof(player->bufferedData[0]) / frameSize;
	ma_uint64 framesRead = 0;
	ma_result res = ma_decoder_read_pcm_frames(&player->minidecoder, destination, framesToRead, &framesRead);
	DWORD flags = 0;
	if(framesRead) {
		if(res == MA_AT_END) {
			flags |= XAUDIO2_END_OF_STREAM;
		}
	} else {
		return 0;
	}

	XAUDIO2_BUFFER submissionBuffer = {0};
	submissionBuffer.AudioBytes = frameSize * framesRead;
	submissionBuffer.pAudioData = (BYTE*)destination;
	submissionBuffer.Flags = flags;
	submissionBuffer.pContext = player;
	IXAudio2SourceVoice_SubmitSourceBuffer(player->audioVoice, &submissionBuffer, NULL);
	atomic_store(&player->state, STREAM_PLAYING);
	return 1;
}

long S_CreateMusicSystem(MUSIC_SYSTEM** out) {
	MUSIC_SYSTEM* sys = calloc(1, sizeof(MUSIC_SYSTEM));
	if(!sys) {
		return 0;
	}
	sys->callbackFuncs = (IXAudio2VoiceCallbackVtbl){
		.OnVoiceError = VoiceError,
		.OnVoiceProcessingPassEnd = VoiceProcessEnded,
		.OnVoiceProcessingPassStart = VoiceProcessStarted,
		.OnBufferStart = BufferStarted,
		.OnBufferEnd = BufferEnded,
		.OnLoopEnd = LoopEnded,
		.OnStreamEnd = StreamEnded,
	};
	sys->callback = (IXAudio2VoiceCallback){
		.lpVtbl = &sys->callbackFuncs
	};

	if(!Check(__func__, XAudio2Create(&sys->x2, XAUDIO2_DEBUG_ENGINE, Processor1))) {
		free(sys);
		return 0;
	}
	XAUDIO2_DEBUG_CONFIGURATION config = { 0 };
	config.LogFunctionName = 1;
	config.LogFileline = 1;
	config.BreakMask = XAUDIO2_LOG_ERRORS;
	config.TraceMask = XAUDIO2_LOG_ERRORS;
	IXAudio2_SetDebugConfiguration(sys->x2, &config, 0);
	if(!Check(__func__, IXAudio2_CreateMasteringVoice(sys->x2, &sys->master, 2, 48000, 0, NULL, NULL, AudioCategory_Media))) {
		IUnknown_Release(sys->x2);
		free(sys);
		return 0;
	}
	*out = sys;
	return 1;
}

void S_ApplyMusicVolume(MUSIC_SYSTEM* sys, long volume) {
	float v = volume / 100.0F;

	IXAudio2MasteringVoice_SetVolume(sys->master,v,XAUDIO2_COMMIT_NOW);
}

MUSIC_SYSTEM* musicImpl;
