#ifndef TOMB4_GAME_DXINFO_H
#define TOMB4_GAME_DXINFO_H

struct DXDIRECTDRAWINFO;
struct DXDIRECTSOUNDINFO;
struct DXINFO {
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
#endif // TOMB4_GAME_DXINFO_H
