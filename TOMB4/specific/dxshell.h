#pragma once
#include "types.h"
#include <guiddef.h>
#include <windows.h>
#include <d3d.h>
struct DXINFO;
struct DXPTR;

void DXBitMask2ShiftCnt(unsigned long mask, unsigned char* shift, unsigned char* count);
void DXReadKeyboard(char* KeyMap);
long DXAttempt(HRESULT r);
void* AddStruct(void* p, long num, long size);
long DXDDCreate(LPGUID pGuid, void** pDD4);
long DXD3DCreate(IDirectDraw4* pDD4, void** pD3D);
long DXSetCooperativeLevel(IDirectDraw4* pDD4, HWND hwnd, long flags);
BOOL __stdcall DXEnumDirectDraw(GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext);
BOOL __stdcall DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);
long DXGetInfo(DXINFO* dxinfo, HWND hwnd);
void DXFreeInfo(DXINFO* dxinfo);
HRESULT __stdcall DXEnumDisplayModes(_DDSURFACEDESC2* lpDDSurfaceDesc2, LPVOID lpContext);
long BPPToDDBD(long BPP);
HRESULT __stdcall DXEnumTextureFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);
HRESULT __stdcall DXEnumZBufferFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);
long DXCreateSurface(IDirectDraw4* dd, _DDSURFACEDESC2* desc, IDirectDrawSurface4** surf);
long DXSetVideoMode(IDirectDraw4* dd, long dwWidth, long dwHeight, long dwBPP);
long DXCreateD3DDevice(IDirect3D3* d3d, GUID guid, IDirectDrawSurface4* surf, IDirect3DDevice3** device);
long DXCreateViewport(IDirect3D3* d3d, IDirect3DDevice3* device, long w, long h, IDirect3DViewport3** viewport);
HRESULT DXShowFrame();
void DXMove(long x, long y);
void DXSaveScreen(IDirectDrawSurface4* surf, const char* name);
void DXClose();
long DXCreate(long w, long h, long bpp, long Flags, DXPTR* dxptr, HWND hWnd, long WindowStyle);
long DXChangeVideoMode();
long DXToggleFullScreen();
HRESULT __stdcall DXEnumDirect3D(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC lpHWDesc, LPD3DDEVICEDESC lpHELDesc, LPVOID lpContext);
const char* DXGetErrorString(HRESULT hr);
void DXInitKeyboard(HWND hwnd, HINSTANCE hinstance);

extern DXPTR* G_dxptr;
extern DXINFO* G_dxinfo;
extern IDirectDraw4* G_ddraw;
extern IDirect3D3* G_d3d;
extern HWND G_hwnd;
extern char keymap[256];
