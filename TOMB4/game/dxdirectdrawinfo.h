#ifndef DXDIRECTDRAWINFO_INCLUDED
#define DXDIRECTDRAWINFO_INCLUDED
#include <guiddef.h>
#include <ddraw.h>

struct DXDISPLAYMODE;
struct DXD3DDEVICE;
struct DXDIRECTDRAWINFO
{
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
};
#endif