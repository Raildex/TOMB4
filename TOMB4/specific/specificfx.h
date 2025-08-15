#ifndef TOMB4_SPECIFIC_SPECIFICFX_H
#define TOMB4_SPECIFIC_SPECIFICFX_H

typedef struct SPARKS SPARKS;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct TEXTURESTRUCT TEXTURESTRUCT;
typedef struct ROPE_STRUCT ROPE_STRUCT;
typedef struct MESH_DATA MESH_DATA;
typedef struct GAME_VECTOR GAME_VECTOR;
typedef struct TEXTURESTRUCT TEXTURESTRUCT;
typedef struct ROPE_STRUCT ROPE_STRUCT;
typedef struct MESH_DATA MESH_DATA;
typedef struct _D3DTLVERTEX _D3DTLVERTEX;
void DrawTrainStrips();
void S_PrintShadow(short size, short* box, ITEM_INFO* item);
void S_DrawDrawSparks(SPARKS* sptr, int smallest_size, int* xyptr, int* zptr);
void DrawBikeSpeedo(int ux, int uy, int vel, int maxVel, int turboVel, int size, int unk);
void Draw2DSprite(int x, int y, int slot, int unused, int unused2);
void DrawJeepSpeedo(int ux, int uy, int vel, int maxVel, int turboVel, int size, int spriteSlot);
void DrawDebris();
void DoScreenFade();
void DrawPsxTile(int x_y, int height_width, int color, int u0, int u1);
void DrawFlash();
void S_DrawDarts(ITEM_INFO* item);
void ClipCheckPoint(_D3DTLVERTEX* v, float x, float y, float z, short* clip);
void DrawFlatSky(unsigned int color, int zpos, int ypos, int drawtype);
void OutputSky();
void ProjectTriPoints(PHD_VECTOR* pos, int* x, int* y, int* z);
void setXY4(_D3DTLVERTEX* v, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int z, short* clip);
void setXY3(_D3DTLVERTEX* v, int x1, int y1, int x2, int y2, int x3, int y3, int z, short* clip);
void setXYZ4(_D3DTLVERTEX* v, int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, int x4, int y4, int z4, short* clip);
void setXYZ3(_D3DTLVERTEX* v, int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, short* clip);
void SetFade(int start, int end);
void DrawLaserSightSprite();
void DrawSprite(int x, int y, int slot, int col, int size, int z);
void ShowTitle();
void SetUpLensFlare(int x, int y, int z, GAME_VECTOR* lfobj);
void InitTarget_2();
void InitBinoculars();
void DrawBinoculars();
void DrawWraithTrail(ITEM_INFO* item);
void DrawDrips();
void DrawBubbles();
void DrawShockwaves();
void DrawTrainFloorStrip(int x, int z, TEXTURESTRUCT* tex, int y_and_flags);
void S_DrawSplashes();
int ClipLine(int* x1, int* y1, int z1, int* x2, int* y2, int z2, int xMin, int yMin, int w, int h);
void S_DrawFireSparks(int size, int life);
void DrawRope(ROPE_STRUCT* rope);
void DrawBlood();
void S_DrawSmokeSparks();
void DoUwEffect();
void DrawLightning();

extern MESH_DATA* targetMeshP;
extern int DoFade;


#endif// TOMB4_SPECIFIC_SPECIFICFX_H
