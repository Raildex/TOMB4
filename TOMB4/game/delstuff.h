#pragma once
struct ITEM_INFO;
struct PHD_VECTOR;
void DrawLara(ITEM_INFO* item, long mirror);
void GetLaraJointPos(PHD_VECTOR* pos, long node);
void SetLaraUnderwaterNodes();
void Rich_CalcLaraMatrices_Normal(short* frame, long* bone, long flag);
void Rich_CalcLaraMatrices_Interpolated(short* frame1, short* frame2, long frac, long rate, long* bone, long flag);
void CalcLaraMatrices(long flag);

extern short* GLaraShadowframe;
extern float lara_matrices[];
extern float lara_joint_matrices[];
extern long LaraNodeAmbient[];
extern long bLaraUnderWater;
extern unsigned char LaraNodeUnderwater[];
extern char SkinVertNums[40][12];
extern char ScratchVertNums[40][12];
extern char HairRotScratchVertNums[5][12];
extern char bLaraInWater;
