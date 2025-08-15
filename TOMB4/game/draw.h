#ifndef TOMB4_GAME_DRAW_H
#define TOMB4_GAME_DRAW_H

#include "game/matrixindices.h"

typedef struct ITEM_INFO ITEM_INFO;
typedef struct ROOM_INFO ROOM_INFO;
typedef struct STATIC_INFO STATIC_INFO;

void InitInterpolate(int frac, int rate);
void phd_PopMatrix_I();
void phd_PushMatrix_I();
void phd_RotY_I(short ang);
void phd_RotX_I(short ang);
void phd_RotZ_I(short ang);
void phd_TranslateRel_I(int x, int y, int z);
void phd_TranslateRel_ID(int x, int y, int z, int x2, int y2, int z2);
void phd_RotYXZ_I(short y, short x, short z);
void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, int skip);
void gar_RotYXZsuperpack(short** pprot, int skip);
void phd_PutPolygons_I(short* ptr, int clip);
void mInterpolateMatrix();
void mInterpolateArmMatrix(float* mx);
void InsertRoom(short room_number);
void CalculateObjectLighting(ITEM_INFO* item, short* frame);
void CalculateObjectLightingLara();
void DrawAnimatingItem(ITEM_INFO* item);
void DrawRooms(short CurrentRoom);
void RenderIt(short CurrentRoom);
int DrawPhaseGame();
void GetRoomBounds();
void SetRoomBounds(short* door, int rn, ROOM_INFO* actualRoom);
void DrawEffect(short fx_num);
void PrintObjects(short room_number);
int GetFrames(ITEM_INFO* item, short* frm[], int* rate);
void GetBoundsAccurate(ITEM_INFO* item, short* result);
short* GetBestFrame(ITEM_INFO* item);
void UpdateSkyLightning();
void mRotBoundingBoxNoPersp(short* bounds, short* rotatedBounds);
void calc_animating_item_clip_window(ITEM_INFO* item, short* bounds);


extern int IM_rate;
extern int IM_frac;

extern float* mIMptr;
extern float mIMstack[indices_count * 64];

extern int current_room;
extern short no_rotation[12];

extern int outside;

extern short SkyPos;
extern short SkyPos2;

extern unsigned short LightningRGB[3];
extern unsigned short LightningRGBs[3];
extern short LightningCount;
extern short dLightningRand;


#endif// TOMB4_GAME_DRAW_H
