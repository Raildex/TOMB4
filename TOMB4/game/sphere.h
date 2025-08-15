#ifndef TOMB4_GAME_SPHERE_H
#define TOMB4_GAME_SPHERE_H
typedef struct SPHERE {
	int x;
	int y;
	int z;
	int r;
} SPHERE;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct PHD_3DPOS PHD_3DPOS;
int GetSpheres(ITEM_INFO* item, SPHERE* ptr, int WorldSpace);
int TestCollision(ITEM_INFO* item, ITEM_INFO* l);
void InitInterpolate2(int frac, int rate);
void GetJointAbsPosition(ITEM_INFO* item, PHD_VECTOR* pos, int joint);
void GetJointAbsPositionRotation(ITEM_INFO* item, PHD_3DPOS* vec, int joint);

extern SPHERE Slist[34];
extern char GotLaraSpheres;

#endif// TOMB4_GAME_SPHERE_H
