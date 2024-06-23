#ifndef TEXTURE_INCLUDED
#define TEXTURE_INCLUDED
struct IDirect3DTexture2;
struct IDirectDrawSurface4;
#pragma pack(push,1)
struct TEXTURE
{
	IDirect3DTexture2* tex;
	IDirectDrawSurface4* surface;
	unsigned long xoff;
	unsigned long yoff;
	unsigned long width;
	unsigned long height;
	long tpage;
	bool bump;
	long bumptpage;
};
#pragma pack(pop)
#endif