
#include "specific/texture.h"
#include "global/types.h"
#include "specific/function_stubs.h"
#include "specific/haltexture.h"
#include "specific/texture.h"
#include "specific/windows/dxd3ddevice.h"
#include "specific/windows/dxdirectdrawinfo.h"
#include "specific/windows/dxflags.h"
#include "specific/windows/dxinfo.h"
#include "specific/windows/dxshell.h"
#include "specific/windows/dxtextureinfo.h"
#include "specific/windows/winapp.h"
#include "specific/windows/winmain.h"
#include <assert.h>
#include <ddraw.h>
#include <math.h>
#include <minwindef.h>
#include <stdlib.h>


typedef struct CreateMipMapFormat0Payload {
	long width;
	long height;
	TEXTURE_FORMAT sourceFmt;
	TEXTURE_FORMAT destinationFmt;
	void* source;
} CreateMipMapFormat0Payload;


void ConvertFormat(void** destination, void* source, TEXTURE_FORMAT tfmt, TEXTURE_FORMAT sfmt) {
	unsigned char sr;
	unsigned char sg;
	unsigned char sb;
	unsigned char sa;
	switch(sfmt) {
	case b8g8r8a8:
		sb = *(((unsigned char*)source));
		sg = *(((unsigned char*)source) + 1);
		sr = *(((unsigned char*)source) + 2);
		sa = *(((unsigned char*)source) + 3);
		break;
	case r4g4b4a4:
		sr = (*(unsigned short*)(source) & 0xF) << 28;
		sg = ((*(unsigned short*)(source) & 0xF0) >> 4) << 28;
		sb = ((*(unsigned short*)(source) & 0xF00) >> 8) << 28;
		sa = ((*(unsigned short*)(source) & 0xF000) >> 12) << 28;
		break;
	case r5g5b5a1:
		sr = (*(unsigned short*)(source) & 0x1F) << 27;
		sg = ((*(unsigned short*)(source) & 0x3E0) >> 5) << 27;
		sb = ((*(unsigned short*)(source) & 0x7C00) >> 10) << 27;
		sa = ((*(unsigned short*)(source) & 0x8000) >> 15) << 31;
		break;
	}
	switch(tfmt) {
	case b8g8r8a8: {
		long* dst = (long*)(*destination);
		*(dst++) = RGBA(sr, sg, sb, sa);
		*destination = dst;
	}

	break;
	case r4g4b4a4: {
		short* dst = (short*)(*destination);
		*(dst++) = (sr & 0xF) | (sg << 4 & 0xF0) | (sb << 8 & 0xF00) | sa << 12 & 0xF000;
		*destination = dst;
	}

	break;
	case r5g5b5a1: {
		short* dst = (short*)(*destination);
		*(dst++) = (sr & 0x1F) | (sg << 5 & 0x3E0) | (sb << 10 & 0x7C00) | (sa << 15 & 0x8000);
		*destination = dst;
	} break;

		break;
	}
}

HRESULT WINAPI CreateMipMapFormat0(LPDIRECTDRAWSURFACE4 surface, LPDDSURFACEDESC2 desc, LPVOID payload) {
	CreateMipMapFormat0Payload* data = (CreateMipMapFormat0Payload*)payload;
	assert(desc->dwWidth);
	assert(desc->dwHeight);
	assert(surface);
	assert(desc);
	assert(data);
	DXAttempt(IDirectDrawSurface4_Lock(surface, 0, desc, DDLOCK_NOSYSLOCK, 0));
	int w = desc->dwWidth;
	int h = desc->dwHeight;
	void* dest = (char*)desc->lpSurface;
	long skip;
	switch(data->sourceFmt) {
	case b8g8r8a8:
		skip = 4;
		break;
	default:
		skip = 2;
		break;
	}
	for(unsigned long y = 0; y < desc->dwHeight; y++) {
		for(unsigned long x = 0; x < desc->dwWidth; x++) {
			long offset = (x * data->width / w + y * (data->width * data->height) / h) * skip;
			ConvertFormat(&dest, ((char*)data->source + offset), data->destinationFmt, data->sourceFmt);
		}
	}
	DXAttempt(IDirectDrawSurface4_Unlock(surface, 0));
	DXAttempt(IDirectDrawSurface4_EnumAttachedSurfaces(surface, payload, CreateMipMapFormat0));
	return DDENUMRET_OK;
}

char CreateTexturePage(long w, long h, TEXTURE_FORMAT tfmt, TEXTURE_FORMAT sfmt, long MipMapCount, void* pSrc, rgbfunc RGBM, HAL_TEXTURE* dst) {
	IDirectDrawSurface4* tSurf;
	DDSURFACEDESC2 desc = { 0 };
	long* lS;
	desc.dwSize = sizeof(DDSURFACEDESC2);
	desc.dwWidth = w;
	desc.dwHeight = h;

	if(w < 32 || h < 32) {
		MipMapCount = 0;
	}

	desc.ddpfPixelFormat = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture].ddpf;
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	if(App.dx.Flags & DXF_HWR) {
		desc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	} else {
		desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
	}

	if(MipMapCount) {
		desc.dwFlags |= DDSD_MIPMAPCOUNT;
		desc.dwMipMapCount = MipMapCount + 1;
		desc.ddsCaps.dwCaps |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
	}

	DXAttempt(DXCreateSurface(App.dx.lpDD, &desc, &tSurf));
	lS = pSrc;
	CreateMipMapFormat0Payload payload = { 0 };
	payload.destinationFmt = tfmt;
	payload.sourceFmt = sfmt;
	payload.source = pSrc;
	payload.width = w;
	payload.height = h;
	DXAttempt(CreateMipMapFormat0(tSurf, &desc, &payload));
	DXAttempt(IDirectDrawSurface4_EnumAttachedSurfaces(tSurf, &payload, &CreateMipMapFormat0));
	DXAttempt(IDirectDrawSurface4_QueryInterface(tSurf, &TEXGUID, (LPVOID*)&dst->dxTex));
	dst->surface = tSurf;
	return 1;
}


void FreeTextures() {
}


long CalcMipMapCount(long w, long h) {
	return (long)floor(log2(max(w, h)));
}