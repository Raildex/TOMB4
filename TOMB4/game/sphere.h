#ifndef TOMB4_GAME_SPHERE_H
#define TOMB4_GAME_SPHERE_H
typedef struct SPHERE {
	long x;
	long y;
	long z;
	long r;
} SPHERE;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct PHD_3DPOS PHD_3DPOS;
long GetSpheres(ITEM_INFO* item, SPHERE* ptr, long WorldSpace);
long TestCollision(ITEM_INFO* item, ITEM_INFO* l);
void InitInterpolate2(long frac, long rate);
void GetJointAbsPosition(ITEM_INFO* item, PHD_VECTOR* pos, long joint);
void GetJointAbsPositionRotation(ITEM_INFO* item, PHD_3DPOS* vec, long joint);

extern SPHERE Slist[34];
extern char GotLaraSpheres;

#endif// TOMB4_GAME_SPHERE_H
