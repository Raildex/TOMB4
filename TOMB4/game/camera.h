#pragma once
#include "game/camerainfo.h"
#include "game/cameratype.h"
#include "game/phdvector.h"
#include "game/shatteritem.h"

typedef struct GAME_VECTOR GAME_VECTOR;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct SHATTER_ITEM SHATTER_ITEM;

void InitialiseCamera();
void MoveCamera(GAME_VECTOR* ideal, long speed);
long mgLOS(GAME_VECTOR* start, GAME_VECTOR* target, long push);
long CameraCollisionBounds(GAME_VECTOR* ideal, long push, long yfirst);
void LaraTorch(PHD_VECTOR* Soffset, PHD_VECTOR* Eoffset, short yrot, long brightness);
void ChaseCamera(ITEM_INFO* item);
void CombatCamera(ITEM_INFO* item);
void LookCamera(ITEM_INFO* item);
void FixedCamera();
void BinocularCamera(ITEM_INFO* item);
void CalculateCamera();

extern CAMERA_INFO camera;

extern GAME_VECTOR ForcedFixedCamera;
extern char UseForcedFixedCamera;

extern PHD_VECTOR LaraTorchStart;
extern PHD_VECTOR LaraTorchEnd;
extern long bLaraTorch;
extern long LaraTorchIntensity;
extern long LaraTorchYRot;

extern camera_type BinocularOldCamera;
extern long BinocularOn;
extern long BinocularRange;
extern long ExittingBinos;
extern long LaserSight;

extern SHATTER_ITEM ShatterItem;
