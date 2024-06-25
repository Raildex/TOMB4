#ifndef WINAPP_INCLUDED
#define WINAPP_INCLUDED
#include "specific/dxinfo.h"
#include "specific/dxptr.h"
#include <windows.h>
#include "global/types.h"
#include <d3dtypes.h>
struct WINAPP {
	HINSTANCE hInstance;
	HWND hWnd;
	WNDCLASS WindowClass;
	DXINFO DXInfo;
	DXPTR dx;
	HANDLE mutex;
	float fps;
	IDirect3DMaterial3* GlobalMaterial;
	D3DMATERIALHANDLE GlobalMaterialHandle;
	HACCEL hAccel;
	bool SetupComplete;
	bool BumpMapping;
	long TextureSize;
	long BumpMapSize;
	bool mmx;
	bool Filtering;
	bool Volumetric;
	bool SoundDisabled;
	long StartFlags;
	volatile bool fmv;
	long Desktopbpp;
	long AutoTarget;
};
#endif