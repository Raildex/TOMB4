#ifndef DXD3DDEVICE_INCLUDED
#define DXD3DDEVICE_INCLUDED
#include <guiddef.h>
#include <d3dcaps.h>

struct DXZBUFFERINFO;
struct DXTEXTUREINFO;
struct DXDISPLAYMODE;
struct DXD3DDEVICE
{
	char Name[30];
	char About[80];
	LPGUID lpGuid;
	GUID Guid;
	D3DDEVICEDESC DeviceDesc;
	long bHardware;
	long nDisplayModes;
	DXDISPLAYMODE* DisplayModes;
	long nTextureInfos;
	DXTEXTUREINFO* TextureInfos;
	long nZBufferInfos;
	DXZBUFFERINFO* ZBufferInfos;
};
#endif