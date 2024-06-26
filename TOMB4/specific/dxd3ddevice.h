#ifndef TOMB4_SPECIFIC_DXD3DDEVICE_H
#define TOMB4_SPECIFIC_DXD3DDEVICE_H
#include <d3d.h>
#include <guiddef.h>

typedef struct DXZBUFFERINFO DXZBUFFERINFO;
typedef struct DXTEXTUREINFO DXTEXTUREINFO;
typedef struct DXDISPLAYMODE DXDISPLAYMODE;
typedef struct DXD3DDEVICE {
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
} DXD3DDEVICE;
#endif// TOMB4_SPECIFIC_DXD3DDEVICE_H
