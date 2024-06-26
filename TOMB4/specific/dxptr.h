#ifndef TOMB4_GAME_DXPTR_H
#define TOMB4_GAME_DXPTR_H
#include <windows.h>
#include <xaudio2.h>
#include <ddraw.h>
#include <d3d.h>
#include <dinput.h>
#include <dsound.h>
typedef struct DXPTR {
	IDirectDraw4* lpDD;
	IDirect3D3* lpD3D;
	IDirect3DDevice3* lpD3DDevice;
	IDirect3DDevice3* _lpD3DDevice;
	IDirectDrawSurface4* lpPrimaryBuffer;
	IDirectDrawSurface4* lpBackBuffer;
	IDirectDrawSurface4* lpZBuffer;
	IDirect3DViewport3* lpViewport;
	IDirectSound8* lpDS;
	IXAudio2* lpXA;
	unsigned long dwRenderWidth;
	unsigned long dwRenderHeight;
	RECT rViewport;
	RECT rScreen;
	long Flags;
	unsigned long WindowStyle;
	long CoopLevel;
	IDirectInput2* lpDirectInput;
	IDirectInputDevice2* Keyboard;
	HWND hWnd;
	volatile long InScene;
	volatile long WaitAtBeginScene;
	volatile long DoneBlit;
} DXPTR;
#endif // TOMB4_GAME_DXPTR_H
