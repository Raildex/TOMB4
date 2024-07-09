#ifndef SPECIFIC_WINDOWS_D3DMATRIX_H
#define SPECIFIC_WINDOWS_D3DMATRIX_H

typedef struct _D3DMATRIX _D3DMATRIX;
typedef struct _D3DVECTOR _D3DVECTOR;


_D3DMATRIX* D3DIdentityMatrix(_D3DMATRIX* mx);
void Set_D3DMATRIX(_D3DMATRIX* mx, float* imx);
void SetD3DViewMatrix();
void D3DTransform(_D3DVECTOR* vec, _D3DMATRIX* mx);
_D3DVECTOR* D3DNormalise(_D3DVECTOR* vec);
void S_Init_D3DMATRIX();
_D3DMATRIX* D3DMultMatrix(_D3DMATRIX* d, _D3DMATRIX* a, _D3DMATRIX* b);

extern _D3DMATRIX D3DMView;
extern _D3DMATRIX D3DLightMatrix;
extern _D3DMATRIX D3DInvCameraMatrix;


#endif// SPECIFIC_WINDOWS_D3DMATRIX_H
