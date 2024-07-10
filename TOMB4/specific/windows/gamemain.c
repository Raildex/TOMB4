
#include "specific/gamemain.h"
#include "game/gameflow.h"
#include "game/savegame.h"
#include "game/savegameinfo.h"
#include "game/sound.h"
#include "game/text.h"
#include "specific/3dmath.h"
#include "specific/audio.h"
#include "specific/drawroom.h"
#include "specific/file.h"
#include "specific/function_stubs.h"
#include "specific/function_table.h"
#include "specific/timing.h"
#include "specific/windows/dxshell.h"
#include "specific/windows/winmain.h"
#include <process.h>


long SaveCounter;

static float unused_vert_wibble_table[256];

void GameClose() {
	Log(__func__, "GameClose");
	ACMClose();
	FreeLevel();

	if(wav_file_buffer) {
		free(wav_file_buffer);
	}

	if(ADPCMBuffer) {
		free(ADPCMBuffer);
	}

	if(logF) {
		fclose(logF);
	}

	free(gfScriptFile);
	free(gfLanguageFile);
}

void S_GameMain() {
	Log(__func__, "GameMain");

	if(S_GameInitialise()) {
		InitialiseFunctionTable();
		HWInitialise();
		InitWindow(0, 0, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 20, 20480, 80, App.dx.dwRenderWidth, App.dx.dwRenderHeight);
		InitFont();
		S_TimeInit();
		App.SetupComplete = 1;
		S_CDStop();
		ClearSurfaces();

		if(!App.SoundDisabled) {
			SOUND_Init();
		}

		DoGameflow();
		GameClose();
		S_CDStop();
		PostMessage(App.hWnd, WM_CLOSE, 0, 0);
	}
}

long S_GameInitialise() {
	S_InitRoomDraw();
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
