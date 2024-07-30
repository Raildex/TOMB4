
#include "specific/fmv.h"
#include "dxptr.h"
#include "game/binkstruct.h"
#include "game/control.h"
#include "game/inputbuttons.h"
#include "game/text.h"
#include "specific/3dmath.h"
#include "specific/audio.h"
#include "specific/function_table.h"
#include "specific/gamemain.h"
#include "specific/input.h"
#include "specific/lighting.h"
#include "specific/loadsave.h"
#include "specific/windows/cmdline.h"
#include "specific/windows/d3dmatrix.h"
#include "specific/windows/dxd3ddevice.h"
#include "specific/windows/dxdirectdrawinfo.h"
#include "specific/windows/dxdisplaymode.h"
#include "specific/windows/dxflags.h"
#include "specific/windows/dxinfo.h"
#include "specific/windows/dxshell.h"
#include "specific/windows/winmain.h"
#include <basetsd.h>
// clang-format off
#include <windows.h>
#include <mmiscapi.h>
#include <mmreg.h>
#include <MSAcm.h>
// clang-format on
#include <dsound.h>
#include <excpt.h>
#include <nb30.h>
#include <stdio.h>

static void(WINAPI* BinkCopyToBuffer)(BINK_STRUCT*, LPVOID, LONG, long, long, long, long);
static void(WINAPI* BinkOpenDirectSound)(unsigned long);
static void(WINAPI* BinkSetSoundSystem)(LPVOID, LPDIRECTSOUND);
static LPVOID(WINAPI* BinkOpen)(char*, unsigned long);
static long(WINAPI* BinkDDSurfaceType)(IDirectDrawSurface4*);
static long(WINAPI* BinkDoFrame)(BINK_STRUCT*);
static void(WINAPI* BinkNextFrame)(BINK_STRUCT*);
static long(WINAPI* BinkWait)(BINK_STRUCT*);
static void(WINAPI* BinkClose)(BINK_STRUCT*);
static HMODULE hBinkW32;

static BINK_STRUCT* Bink;
static IDirectDrawSurface4* BinkSurface;
static long BinkSurfaceType;

#define GET_DLL_PROC(dll, proc, n)                            \
	{                                                         \
		*(FARPROC*)&(proc) = GetProcAddress((dll), n);        \
		if(!proc)                                             \
			RaiseException(1, 2, 1, (const ULONG_PTR*)#proc); \
	}

long LoadBinkStuff() {
	hBinkW32 = LoadLibrary("binkw32.dll");

	if(!hBinkW32) {
		return 0;
	}

	__try {
		GET_DLL_PROC(hBinkW32, BinkCopyToBuffer, "_BinkCopyToBuffer@28");
		GET_DLL_PROC(hBinkW32, BinkOpenDirectSound, "_BinkOpenDirectSound@4");
		GET_DLL_PROC(hBinkW32, BinkSetSoundSystem, "_BinkSetSoundSystem@8");
		GET_DLL_PROC(hBinkW32, BinkOpen, "_BinkOpen@8");
		GET_DLL_PROC(hBinkW32, BinkDDSurfaceType, "_BinkDDSurfaceType@4");
		GET_DLL_PROC(hBinkW32, BinkDoFrame, "_BinkDoFrame@4");
		GET_DLL_PROC(hBinkW32, BinkNextFrame, "_BinkNextFrame@4");
		GET_DLL_PROC(hBinkW32, BinkWait, "_BinkWait@4");
		GET_DLL_PROC(hBinkW32, BinkClose, "_BinkClose@4");
	} __except(GetExceptionCode() == 1 ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
		FreeLibrary(hBinkW32);
		hBinkW32 = 0;
		return 0;
	}

	return 1;
}

void FreeBinkStuff() {
	if(hBinkW32) {
		FreeLibrary(hBinkW32);
		hBinkW32 = 0;
	}
}

void ShowBinkFrame() {
	DDSURFACEDESC2 surf = { 0 };

	surf.dwSize = sizeof(DDSURFACEDESC2);
	DXAttempt(IDirectDrawSurface4_Lock(BinkSurface, 0, &surf, DDLOCK_NOSYSLOCK, 0));
	BinkCopyToBuffer(Bink, surf.lpSurface, surf.lPitch, Bink->num, 0, 0, BinkSurfaceType);
	DXAttempt(IDirectDrawSurface4_Unlock(BinkSurface, 0));

	if(App.dx.Flags & DXF_WINDOWED) {
		DXShowFrame();
	}
}

long PlayFmvNow(long num) {
	return 0;
	DXDISPLAYMODE* modes;
	DXDISPLAYMODE* current;
	long dm, rm, ndms;
	char name[80];
	char path[80];

	if((1 << num) & FmvSceneTriggered) {
		return 1;
	}

	FmvSceneTriggered |= 1 << num;
	S_CDStop();

	if(fmvs_disabled) {
		return 0;
	}

	sprintf(name, "fmv\\fmv%02ld.bik", num);
	memset(path, 0, sizeof(path));
	strcat(path, name);
	App.fmv = 1;
	modes = G_dxinfo->DDInfo[App.DXInfo.nDD].D3DDevices[App.DXInfo.nD3D].DisplayModes;
	rm = 0;
	dm = App.DXInfo.nDisplayMode;
	current = &modes[dm];

	if(current->bpp != 32 || current->w != 640 || current->h != 480) {
		ndms = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes;

		for(int i = 0; i < ndms; i++, modes++) {
			if(modes->bpp == 32 && modes->w == 640 && modes->h == 480) {
				App.DXInfo.nDisplayMode = i;
				break;
			}
		}

		DXChangeVideoMode();
		HWInitialise();
		ClearSurfaces();
		rm = 1;
	}

	Bink = 0;
	IDirectSound* lpDS;
	IUnknown_QueryInterface(App.dx.lpDS, &IID_IDirectSound, (LPVOID)&lpDS);
	BinkSetSoundSystem(BinkOpenDirectSound, lpDS);
	Bink = (BINK_STRUCT*)BinkOpen(path, 0);

	if(App.dx.Flags & DXF_WINDOWED) {
		BinkSurface = App.dx.lpBackBuffer;
	} else {
		BinkSurface = App.dx.lpPrimaryBuffer;
	}

	BinkSurfaceType = BinkDDSurfaceType(BinkSurface);

	if(Bink) {
		BinkDoFrame(Bink);
		S_UpdateInput(inputImpl);

		for(int i = 0; i != Bink->num2; i++) {
			if(S_IsActionDown(inputImpl, IN_DESELECT)) {
				break;
			}

			BinkNextFrame(Bink);

			while(BinkWait(Bink))
				;

			ShowBinkFrame();
			BinkDoFrame(Bink);
			S_UpdateInput(inputImpl);
		}


		BinkClose(Bink);
		Bink = 0;
	}

	if(rm) {
		App.DXInfo.nDisplayMode = dm;
		DXChangeVideoMode();
		InitWindow(0, 0, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 20, 20480, 80, App.dx.dwRenderWidth, App.dx.dwRenderHeight);
		InitFont();
		S_Init_D3DMATRIX();
		SetD3DViewMatrix();
	}

	//DXChangeOutputFormat(sfx_frequencies[SoundQuality], 1);
	HWInitialise();
	ClearSurfaces();
	App.fmv = 0;
	return 0;
}
