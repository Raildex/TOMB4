#ifndef TOMB4_SPECIFIC_DXDIRECTDRAWINFO_H
#define TOMB4_SPECIFIC_DXDIRECTDRAWINFO_H
#include <guiddef.h>
#include <ddraw.h>

typedef struct DXDISPLAYMODE DXDISPLAYMODE;
typedef struct DXD3DDEVICE DXD3DDEVICE;
typedef struct DXDIRECTDRAWINFO {
	char Name[30];
	char About[80];
	LPGUID lpGuid;
	GUID Guid;
	DDCAPS DDCaps;
	DDDEVICEIDENTIFIER DDIdentifier;
	long nDisplayModes;
	DXDISPLAYMODE* DisplayModes;
	long nD3DDevices;
	DXD3DDEVICE* D3DDevices;
} DXDIRECTDRAWINFO;
#endif// TOMB4_SPECIFIC_DXDIRECTDRAWINFO_H
