
#include "specific/texture.h"
#include "specific/dxshell.h"
#include "specific/function_stubs.h"
#include "specific/winmain.h"
#include <cassert>
#include <ddraw.h>
#include "specific/texture.h"
#include "specific/dxinfo.h"
#include "specific/dxdirectdrawinfo.h"
#include "specific/dxd3ddevice.h"
#include "specific/dxflags.h"
#include "specific/dxtextureinfo.h"

TEXTURE* Textures;
long nTextures;
HRESULT WINAPI CreateMipMapFormat1(LPDIRECTDRAWSURFACE4 surface, LPDDSURFACEDESC2 desc, LPVOID source) {
	assert(desc->dwWidth == desc->dwHeight);
	assert(surface);
	assert(desc);
	assert(source);
	DXAttempt(surface->Lock(0, desc, DDLOCK_NOSYSLOCK, 0));
	Log(1, "Mip Map Surface found of size %d", desc->dwWidth);
	int w = desc->dwWidth;
	int h = desc->dwHeight;
	int offset = 256 / desc->dwWidth;
	long* lD = (long*)desc->lpSurface;
	for(unsigned long y = 0; y < desc->dwHeight; y++) {
		for(unsigned long x = 0; x < desc->dwWidth; x++) {
			long value = *((long*)source + x * 256 / w + y * 0x10000 / h);
			*lD++ = value;
		}
	}
	DXAttempt(surface->Unlock(0));
	DXAttempt(surface->EnumAttachedSurfaces(source, CreateMipMapFormat1));
	return DDENUMRET_OK;
}

struct CreateMipMapFormat0Payload {
	unsigned long rbpp;
	unsigned long bbpp;
	unsigned long gbpp;
	unsigned long abpp;
	unsigned long rshift;
	unsigned long gshift;
	unsigned long bshift;
	unsigned long ashift;
	unsigned long bpp;
	long* lS;
};
HRESULT WINAPI CreateMipMapFormat0(LPDIRECTDRAWSURFACE4 surface, LPDDSURFACEDESC2 desc, LPVOID payload) {
	assert(desc->dwWidth == desc->dwHeight);
	CreateMipMapFormat0Payload* data = (CreateMipMapFormat0Payload*)payload;

	DXAttempt(surface->Lock(0, desc, DDLOCK_NOSYSLOCK, 0));
	Log(1, "Mip Map Surface found of size %d", desc->dwWidth);
	int w = desc->dwWidth;
	int h = desc->dwHeight;
	int offset = 256 / desc->dwWidth;
	long* lS = (long*)data->lS;
	char* cD = (char*)desc->lpSurface;
	for(unsigned long y = 0; y < desc->dwHeight; y++) {
		for(unsigned long x = 0; x < desc->dwWidth; x++) {
			unsigned long c, o, ro, go, bo, ao;
			unsigned char r, g, b, a;
			c = *(lS + x * 256 / w + y * 0x10000 / h);
			r = CLRR(c);
			g = CLRG(c);
			b = CLRB(c);
			a = CLRA(c);

			ro = r >> (8 - data->rbpp) << (data->rshift);
			go = g >> (8 - data->gbpp) << (data->gshift);
			bo = b >> (8 - data->bbpp) << (data->bshift);
			ao = a >> (8 - data->abpp) << (data->ashift);
			o = ro | go | bo | ao;

			for(int i = data->bpp; i > 0; i -= 8) {
				*cD++ = (char)o;
				o >>= 8;
			}
		}
	}
	DXAttempt(surface->Unlock(0));
	DXAttempt(surface->EnumAttachedSurfaces(payload, CreateMipMapFormat0));
	return DDENUMRET_OK;
}
HRESULT WINAPI CreateMipMapFormat2(LPDIRECTDRAWSURFACE4 surface, LPDDSURFACEDESC2 desc, LPVOID source) {
	assert(desc->dwWidth == desc->dwHeight);
	DXAttempt(surface->Lock(0, desc, DDLOCK_NOSYSLOCK, 0));
	Log(1, "Mip Map Surface found of size %d", desc->dwWidth);
	int w = desc->dwWidth;
	int h = desc->dwHeight;
	int offset = 256 / desc->dwWidth;
	short* sS = (short*)source;
	short* sD = (short*)desc->lpSurface;
	for(unsigned long y = 0; y < desc->dwHeight; y++) {
		for(unsigned long x = 0; x < desc->dwWidth; x++)
			*sD++ = *(sS + x * 256 / w + y * 0x10000 / h);
	}

	DXAttempt(surface->Unlock(0));
	DXAttempt(surface->EnumAttachedSurfaces(source, CreateMipMapFormat2));
	return DDENUMRET_OK;
}

IDirectDrawSurface4* CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format) {
	DXTEXTUREINFO* tex;
	IDirectDrawSurface4* tSurf;
	DDSURFACEDESC2 desc;
	long* lS;
	long* lD;
	short* sS;
	short* sD;
	char* cD;
	unsigned long c, o, ro, go, bo, ao;
	unsigned char r, g, b, a;

	memset(&desc, 0, sizeof(DDSURFACEDESC2));
	desc.dwSize = sizeof(DDSURFACEDESC2);
	desc.dwWidth = w;
	desc.dwHeight = h;

	if(w < 32 || h < 32)
		MipMapCount = 0;

	desc.ddpfPixelFormat = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture].ddpf;
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

	if(App.dx.Flags & DXF_HWR)
		desc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	else
		desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	if(MipMapCount) {
		desc.dwFlags |= DDSD_MIPMAPCOUNT;
		desc.dwMipMapCount = MipMapCount + 1;
		desc.ddsCaps.dwCaps |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
	}

	DXCreateSurface(App.dx.lpDD, &desc, &tSurf);
	// DXAttempt(tSurf->Lock(0, &desc, DDLOCK_NOSYSLOCK, 0));

	if(!format) {
		lS = pSrc;
		cD = (char*)desc.lpSurface;

		tex = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture];
		CreateMipMapFormat0Payload payload;
		ZeroMemory(&payload, sizeof(CreateMipMapFormat0Payload));
		payload.rbpp = tex->rbpp;
		payload.gbpp = tex->gbpp;
		payload.bbpp = tex->bbpp;
		payload.abpp = tex->abpp;
		payload.rshift = tex->rshift;
		payload.gshift = tex->gshift;
		payload.bshift = tex->bshift;
		payload.ashift = tex->ashift;
		payload.lS = lS;
		payload.bpp = tex->bpp;
		DXAttempt(CreateMipMapFormat0(tSurf, &desc, &payload));
		DXAttempt(tSurf->EnumAttachedSurfaces(&payload, CreateMipMapFormat0));
	} else if(format == 2) {
		DXAttempt(CreateMipMapFormat2(tSurf, &desc, pSrc));
		DXAttempt(tSurf->EnumAttachedSurfaces(pSrc, CreateMipMapFormat2));

	} else if(format == 1) {
		DXAttempt(CreateMipMapFormat1(tSurf, &desc, pSrc));
		DXAttempt(tSurf->EnumAttachedSurfaces(pSrc, CreateMipMapFormat1));
	}

	// DXAttempt(tSurf->Unlock(0));
	return tSurf;
}


void FreeTextures() {
	TEXTURE* tex;

	DXAttempt(App.dx.lpD3DDevice->SetTexture(0, 0));
	DXAttempt(App.dx.lpD3D->EvictManagedTextures());

	for(int i = 0; i < nTextures; i++) {
		tex = &Textures[i];

		if(tex->tex) {
			Log(4, "Released %s @ %x - RefCnt = %d", "Texture", tex->tex, tex->tex->Release());
			tex->tex = 0;
		} else
			Log(1, "%s Attempt To Release NULL Ptr", "Texture");

		if(tex->surface) {
			Log(4, "Released %s @ %x - RefCnt = %d", "Surface", tex->surface, tex->surface->Release());
			tex->surface = 0;
		} else
			Log(1, "%s Attempt To Release NULL Ptr", "Surface");
	}

	free(Textures);
	Textures = 0;
}
