#ifndef TOMB4_SPECIFIC_3DMATH_H
#define TOMB4_SPECIFIC_3DMATH_H
#include "game/fvector.h"
#include "game/matrixindices.h"
#include "global/math_tbls.h"
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct PHD_3DPOS PHD_3DPOS;
void phd_PushMatrix();
void phd_PushUnitMatrix();
void phd_SetTrans(int x, int y, int z);
int phd_TranslateRel(int x, int y, int z);
void phd_RotX(short angle);
void phd_RotY(short angle);
void phd_RotZ(short angle);
void phd_RotYXZpack(int angles);
void phd_RotYXZ(short y, short x, short z);
void phd_TranslateAbs(int x, int y, int z);
void phd_GetVectorAngles(int x, int y, int z, short* angles);
unsigned int mGetAngle(int x, int z, int x1, int z1);
void AlterFOV(short fov);
int phd_atan(int x, int y);
unsigned int phd_sqrt(unsigned int num);
void ScaleCurrentMatrix(PHD_VECTOR* vec);
void SetupZRange(int znear, int zfar);
void InitWindow(int x, int y, int w, int h, int znear, int zfar, int fov, int a, int b);
void phd_GenerateW2V(PHD_3DPOS* viewPos);
void phd_LookAt(int sx, int sy, int sz, int tx, int ty, int tz, short roll);
FVECTOR VectorNormalise(FVECTOR* vec);
float VectorMagnitude(FVECTOR* vec);
extern float one;
extern float mone;
extern float FogStart;
extern float FogEnd;

extern float* mMXPtr;
extern float mW2V[indices_count];
extern float fMXStack[20 * indices_count];
extern float fcossin_tbl[65536];

extern long* phd_mxptr;
extern int w2v_matrix[indices_count];
extern int matrix_stack[20 * indices_count];

extern float f_centerx;
extern float f_centery;
extern float f_top;
extern float f_left;
extern float f_bottom;
extern float f_right;
extern float f_znear;
extern float f_zfar;
extern float f_mznear;
extern float f_mzfar;
extern float f_persp;
extern float f_mpersp;
extern float f_oneopersp;
extern float f_moneopersp;
extern float f_perspoznear;
extern float f_mperspoznear;
extern float f_moneoznear;
extern float f_a;
extern float f_b;
extern float f_boo;

extern int phd_winheight;
extern int phd_winwidth;
extern int phd_centerx;
extern int phd_centery;
extern int phd_top;
extern int phd_left;
extern int phd_bottom;
extern int phd_right;
extern int phd_znear;
extern int phd_zfar;
extern int phd_persp;
extern short phd_winxmax;
extern short phd_winxmin;
extern short phd_winymax;
extern short phd_winymin;

__inline short phd_sin(int angle) {
	angle >>= 3;
	return 4 * rcossin_tbl[angle & 0x1FFE];
}

__inline short phd_cos(int angle) {
	angle >>= 3;
	return 4 * rcossin_tbl[(angle & 0x1FFE) + 1];
}

__inline float fSin(int angle) {
	return fcossin_tbl[(unsigned short)angle];
}

__inline float fCos(int angle) {
	return fcossin_tbl[(unsigned short)(angle + 0x4000)];
}

__inline void mPopMatrix() {
	mMXPtr -= indices_count;
}

__inline void phd_PopMatrix() {
	phd_mxptr -= indices_count;
	mPopMatrix();
}


#endif// TOMB4_SPECIFIC_3DMATH_H
