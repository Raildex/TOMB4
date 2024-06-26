#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED
typedef struct ITEM_INFO ITEM_INFO;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct SPHERE SPHERE;
long GetSpheres(ITEM_INFO* item, SPHERE* ptr, long WorldSpace);
long TestCollision(ITEM_INFO* item, ITEM_INFO* l);
void InitInterpolate2(long frac, long rate);
void GetJointAbsPosition(ITEM_INFO* item, PHD_VECTOR* pos, long joint);

extern SPHERE Slist[34];
extern char GotLaraSpheres;
typedef struct SPHERE {
	long x;
	long y;
	long z;
	long r;
} SPHERE;
#endif