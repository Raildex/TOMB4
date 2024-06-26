#ifndef TOMB4_SPECIFIC_DXINFO_H
#define TOMB4_SPECIFIC_DXINFO_H
#include <stdbool.h>
typedef struct DXDIRECTDRAWINFO DXDIRECTDRAWINFO;
typedef struct DXDIRECTSOUNDINFO DXDIRECTSOUNDINFO;
typedef struct DXINFO {
	long nDDInfo;
	long nDSInfo;
	DXDIRECTDRAWINFO* DDInfo;
	DXDIRECTSOUNDINFO* DSInfo;
	long nDD;
	long nD3D;
	long nDisplayMode;
	long nTexture;
	long nDS;
} DXINFO;
#endif// TOMB4_SPECIFIC_DXINFO_H
