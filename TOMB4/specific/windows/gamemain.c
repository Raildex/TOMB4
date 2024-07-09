
#include "specific/gamemain.h"
#include "specific/audio.h"
#include "specific/file.h"
#include "specific/function_stubs.h"
#include "specific/function_table.h"
#include "specific/3dmath.h"
#include "game/text.h"
#include "specific/windows/winmain.h"
#include "game/sound.h"
#include "game/gameflow.h"
#include "specific/windows/dxshell.h"
#include "game/savegame.h"
#include "game/watertab.h"
#include "game/savegameinfo.h"
#include "specific/timing.h"
#include <process.h>

LPDIRECT3DVERTEXBUFFER DestVB;
WATERTAB WaterTable[22][64];
short* clipflags;
float vert_wibble_table[32];
long SaveCounter;

static float unused_vert_wibble_table[256];
static unsigned char water_abs[4] = { 4, 8, 12, 16 };
static short water_shimmer[4] = { 31, 63, 95, 127 };
static short water_choppy[4] = { 16, 53, 90, 127 };

void GameClose() {
	Log(__func__, "GameClose");
	ACMClose();
	FreeLevel();

	if(DestVB) {
		Log(__func__, "Released %s @ %x - RefCnt = %d", "Dest VB", DestVB, IDirect3DVertexBuffer_Release(DestVB));
		DestVB = 0;
	} else
		Log(__func__, "%s Attempt To Release NULL Ptr", "Dest VB");

	free(clipflags);

	if(wav_file_buffer)
		free(wav_file_buffer);

	if(ADPCMBuffer)
		free(ADPCMBuffer);

	if(logF)
		fclose(logF);

	free(gfScriptFile);
	free(gfLanguageFile);
}

unsigned int WINAPI GameMain(void* ptr) {
	Log(__func__, "GameMain");

	if(GameInitialise()) {
		InitialiseFunctionTable();
		HWInitialise();
		InitWindow(0, 0, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 20, 20480, 80, App.dx.dwRenderWidth, App.dx.dwRenderHeight);
		InitFont();
		S_TimeInit();
		App.SetupComplete = 1;
		S_CDStop();
		ClearSurfaces();

		if(!App.SoundDisabled)
			SOUND_Init();

		DoGameflow();
		GameClose();
		S_CDStop();
		PostMessage(App.hWnd, WM_CLOSE, 0, 0);
	}

	return 1;
}

unsigned short GetRandom(WATERTAB* wt, long lp) {
	long loop;
	unsigned short ret;

	do {
		ret = rand() & 0xFC;

		for(loop = 0; loop < lp; loop++)
			if(wt[loop].random == ret)
				break;

	} while(loop != lp);

	return ret;
}

void init_water_table() {
	float fSin;
	long lSin;
	short sSin, angle;

	srand(121197);

	for(int i = 0; i < 64; i++) {
		sSin = rcossin_tbl[i << 7];
		WaterTable[0][i].shimmer = (63 * sSin) >> 15;
		WaterTable[0][i].choppy = (16 * sSin) >> 12;
		WaterTable[0][i].random = (unsigned char)GetRandom(&WaterTable[0][0], i);
		WaterTable[0][i].abs = 0;

		WaterTable[1][i].shimmer = (32 * sSin) >> 15;
		WaterTable[1][i].choppy = 0;
		WaterTable[1][i].random = (unsigned char)GetRandom(&WaterTable[1][0], i);
		WaterTable[1][i].abs = -3;

		WaterTable[2][i].shimmer = (64 * sSin) >> 15;
		WaterTable[2][i].choppy = 0;
		WaterTable[2][i].random = (unsigned char)GetRandom(&WaterTable[2][0], i);
		WaterTable[2][i].abs = 0;

		WaterTable[3][i].shimmer = (96 * sSin) >> 15;
		WaterTable[3][i].choppy = 0;
		WaterTable[3][i].random = (unsigned char)GetRandom(&WaterTable[3][0], i);
		WaterTable[3][i].abs = 4;

		WaterTable[4][i].shimmer = (127 * sSin) >> 15;
		WaterTable[4][i].choppy = 0;
		WaterTable[4][i].random = (unsigned char)GetRandom(&WaterTable[4][0], i);
		WaterTable[4][i].abs = 8;

		for(int j = 0, k = 5; j < 4; j++, k += 4) {
			for(int m = 0; m < 4; m++) {
				WaterTable[k + m][i].shimmer = -((sSin * water_shimmer[m]) >> 15);
				WaterTable[k + m][i].choppy = sSin * water_choppy[j] >> 12;
				WaterTable[k + m][i].random = (unsigned char)GetRandom(&WaterTable[k + m][0], i);
				WaterTable[k + m][i].abs = water_abs[m];
			}
		}
	}

	for(int i = 0; i < 32; i++) {
		fSin = sinf((float)(i * (M_PI / 16.0F)));
		vert_wibble_table[i] = fSin + fSin;
	}

	for(int i = 0; i < 256; i++) {
		angle = 0x10000 * i / 256;
		lSin = phd_sin(angle);
		unused_vert_wibble_table[i] = (float)(lSin >> (W2V_SHIFT - 5));
	}
}

bool GameInitialise() {
	D3DVERTEXBUFFERDESC desc;

	desc.dwCaps = 0;
	desc.dwSize = sizeof(desc);
	desc.dwFVF = D3DFVF_TLVERTEX;
	desc.dwNumVertices = 0xFFFF;
	DXAttempt(IDirect3D3_CreateVertexBuffer(App.dx.lpD3D,&desc, &DestVB, D3DDP_DONOTCLIP, 0));
	clipflags = (short*)calloc(0x4000,1);
	init_water_table();
	return 1;
}

long S_SaveGame(long slot_num) {
	HANDLE file;
	unsigned long bytes;
	long days, hours, minutes, seconds;
	char buffer[80], counter[16];

	memset(buffer, 0, sizeof(buffer));
	wsprintf(buffer, "savegame.%d", slot_num);
	file = CreateFile(buffer, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if(file != INVALID_HANDLE_VALUE) {
		memset(buffer, 0, sizeof(buffer));
		wsprintf(buffer, "%s", SCRIPT_TEXT(gfLevelNames[gfCurrentLevel]));
		WriteFile(file, buffer, 75, &bytes, 0);
		WriteFile(file, &SaveCounter, sizeof(long), &bytes, 0);
		days = savegame.Game.Timer / 30 / 86400;
		hours = savegame.Game.Timer / 30 % 86400 / 3600;
		minutes = savegame.Game.Timer / 30 / 60 % 60;
		seconds = savegame.Game.Timer / 30 % 60;
		WriteFile(file, &days, 2, &bytes, 0);
		WriteFile(file, &hours, 2, &bytes, 0);
		WriteFile(file, &minutes, 2, &bytes, 0);
		WriteFile(file, &seconds, 2, &bytes, 0);
		WriteFile(file, &savegame, sizeof(SAVEGAME_INFO), &bytes, 0);
		CloseHandle(file);
		wsprintf(counter, "%d", SaveCounter);
		SaveCounter++;
		return 1;
	}

	return 0;
}

long S_LoadGame(long slot_num) {
	HANDLE file;
	unsigned long bytes;
	long value;
	char buffer[80];

	wsprintf(buffer, "savegame.%d", slot_num);
	file = CreateFile(buffer, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if(file != INVALID_HANDLE_VALUE) {
		ReadFile(file, buffer, 75, &bytes, 0);
		ReadFile(file, &value, sizeof(long), &bytes, 0);
		ReadFile(file, &value, sizeof(long), &bytes, 0);
		ReadFile(file, &value, sizeof(long), &bytes, 0);
		ReadFile(file, &savegame, sizeof(SAVEGAME_INFO), &bytes, 0);
		CloseHandle(file);
		return 1;
	}

	return 0;
}
