#ifndef TOMB4_GAME_CAMERA_H
#define TOMB4_GAME_CAMERA_H
#include "game/camerainfo.h"
#include "game/cameratype.h"
#include "game/phdvector.h"
#include "game/shatteritem.h"

typedef struct GAME_VECTOR GAME_VECTOR;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct SHATTER_ITEM SHATTER_ITEM;

void InitialiseCamera();
void LaraTorch(PHD_VECTOR* Soffset, PHD_VECTOR* Eoffset, short yrot, int brightness);
void CalculateCamera();

extern CAMERA_INFO camera;

extern GAME_VECTOR ForcedFixedCamera;
extern char UseForcedFixedCamera;

extern camera_type BinocularOldCamera;
extern int BinocularOn;
extern int BinocularRange;
extern int ExittingBinos;
extern int LaserSight;



#endif// TOMB4_GAME_CAMERA_H
