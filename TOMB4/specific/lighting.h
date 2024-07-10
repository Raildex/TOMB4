#ifndef TOMB4_SPECIFIC_LIGHTING_H
#define TOMB4_SPECIFIC_LIGHTING_H
#include "game/pointlightstruct.h"
#include "game/sunlightstruct.h"

typedef struct ITEM_INFO ITEM_INFO;
typedef struct FVECTOR FVECTOR;
typedef struct ROOM_INFO ROOM_INFO;
typedef struct PCLIGHT PCLIGHT;
typedef struct DYNAMIC DYNAMIC;


void S_CalculateStaticMeshLight(long x, long y, long z, long shade, ROOM_INFO* r);
void InitItemDynamicLighting(ITEM_INFO* item);
void SetupDynamicLight(DYNAMIC* light, ITEM_INFO* item);
void SetupLight(PCLIGHT* light, ITEM_INFO* item, long* ambient);
void mApplyMatrix(float* matrix, FVECTOR* start, FVECTOR* dest);
void mApplyTransposeMatrix(float* matrix, FVECTOR* start, FVECTOR* dest);
void CreateLightList(ITEM_INFO* item);
void FadeLightList(PCLIGHT* lights, long nLights);
void InitObjectLighting(ITEM_INFO* item);
void CalcAmbientLight(ITEM_INFO* item);
void ResetLighting();

extern ITEM_INFO* current_item;
extern long StaticMeshShade;
extern long ambientR, ambientG, ambientB;

extern FVECTOR lGlobalMeshPos;
extern SUNLIGHT_STRUCT SunLights[64];
extern POINTLIGHT_STRUCT PointLights[64];
extern POINTLIGHT_STRUCT SpotLights[64];
extern long nSunLights, nPointLights, nSpotLights, nShadowLights, nTotalLights;


#endif// TOMB4_SPECIFIC_LIGHTING_H
