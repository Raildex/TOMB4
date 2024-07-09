#ifndef TOMB4_GAME_DXPTR_H
#define TOMB4_GAME_DXPTR_H
#include <windows.h>
typedef struct IDirectDraw4 IDirectDraw4;
typedef struct IDirect3D3 IDirect3D3;
typedef struct IDirect3DDevice3 IDirect3DDevice3;
typedef struct IDirectDrawSurface4 IDirectDrawSurface4;
typedef struct IDirect3DViewport3 IDirect3DViewport3;
typedef struct IDirectSound8 IDirectSound8;
typedef struct IXAudio2 IXAudio2;
typedef struct IDirectInputDevice2A IDirectInputDevice2A;
typedef struct IDirectInput2A IDirectInput2A;
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
	IDirectInput2A* lpDirectInput;
	IDirectInputDevice2A* Keyboard;
	HWND hWnd;
	volatile long InScene;
	volatile long WaitAtBeginScene;
	volatile long DoneBlit;
} DXPTR;
#endif // TOMB4_GAME_DXPTR_H
