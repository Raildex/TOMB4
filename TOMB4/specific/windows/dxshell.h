#ifndef SPECIFIC_WINDOWS_DXSHELL_H
#define SPECIFIC_WINDOWS_DXSHELL_H
#include "global/types.h"
#include <guiddef.h>
#include <windows.h>
#include <d3d.h>
typedef struct DXINFO DXINFO;
typedef struct DXPTR DXPTR;

void DXBitMask2ShiftCnt(unsigned int mask, unsigned char* shift, unsigned char* count);
void DXReadKeyboard(char* KeyMap);
int DXAttempt(HRESULT r);
void* AddStruct(void* p, int num, int size);
int DXDDCreate(LPGUID pGuid, void** pDD4);
int DXD3DCreate(IDirectDraw4* pDD4, void** pD3D);
int DXSetCooperativeLevel(IDirectDraw4* pDD4, HWND hwnd, int flags);
BOOL WINAPI DXEnumDirectDraw(GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext);
BOOL WINAPI DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);
int DXGetInfo(DXINFO* dxinfo, HWND hwnd);
void DXFreeInfo(DXINFO* dxinfo);
HRESULT WINAPI DXEnumDisplayModes(DDSURFACEDESC2* lpDDSurfaceDesc2, LPVOID lpContext);
int BPPToDDBD(int BPP);
HRESULT WINAPI DXEnumTextureFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);
HRESULT WINAPI DXEnumZBufferFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);
int DXCreateSurface(IDirectDraw4* dd, DDSURFACEDESC2* desc, IDirectDrawSurface4** surf);
int DXSetVideoMode(IDirectDraw4* dd, int dwWidth, int dwHeight, int dwBPP);
int DXCreateD3DDevice(IDirect3D3* d3d, GUID guid, IDirectDrawSurface4* surf, IDirect3DDevice3** device);
int DXCreateViewport(IDirect3D3* d3d, IDirect3DDevice3* device, int w, int h, IDirect3DViewport3** viewport);
HRESULT DXShowFrame();
void DXMove(int x, int y);
void DXSaveScreen(IDirectDrawSurface4* surf, const char* name);
void DXClose();
int DXCreate(int w, int h, int bpp, int Flags, DXPTR* dxptr, HWND hWnd, int WindowStyle);
int DXChangeVideoMode();
int DXToggleFullScreen();
HRESULT WINAPI DXEnumDirect3D(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC lpHWDesc, LPD3DDEVICEDESC lpHELDesc, LPVOID lpContext);
const char* DXGetErrorString(HRESULT hr);
void DXInitKeyboard(HWND hwnd, HINSTANCE hinstance);

extern DXPTR* G_dxptr;
extern DXINFO* G_dxinfo;
extern IDirectDraw4* G_ddraw;
extern IDirect3D3* G_d3d;
extern HWND G_hwnd;
extern char keymap[256];


#endif// SPECIFIC_WINDOWS_DXSHELL_H
