#ifndef DXINFO_INCLUDED
#define DXINFO_INCLUDED

struct DXDIRECTDRAWINFO;
struct DXDIRECTSOUNDINFO;
struct DXINFO
{
	long nDDInfo;
	long nDSInfo;
	DXDIRECTDRAWINFO* DDInfo;
	DXDIRECTSOUNDINFO* DSInfo;
	long nDD;
	long nD3D;
	long nDisplayMode;
	long nTexture;
	long nZBuffer;
	long nDS;
	bool bHardware;
};
#endif