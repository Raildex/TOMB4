#ifndef TOMB4_GAME_CONTROL_H
#define TOMB4_GAME_CONTROL_H
#include "game/heighttypes.h"
#include "game/shatteritem.h"
typedef struct ROOM_INFO ROOM_INFO;
typedef struct FLOOR_INFO FLOOR_INFO;
typedef struct ITEM_INFO ITEM_INFO;
typedef struct ANIM_STRUCT ANIM_STRUCT;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct MESH_INFO MESH_INFO;
typedef struct PHD_3DPOS PHD_3DPOS;
typedef struct GAME_VECTOR GAME_VECTOR;
enum no_height {
	NO_HEIGHT = -32512
};
int ControlPhase(int nframes, int demo_mode);
void FlipMap(int FlipNumber);
void TestTriggers(short* data, int heavy, int HeavyFlags);
short GetDoor(FLOOR_INFO* floor);
int CheckNoColFloorTriangle(FLOOR_INFO* floor, int x, int z);
int CheckNoColCeilingTriangle(FLOOR_INFO* floor, int x, int z);
FLOOR_INFO* GetFloor(int x, int y, int z, short* room_number);
int GetWaterHeight(int x, int y, int z, short room_number);
int GetHeight(FLOOR_INFO* floor, int x, int y, int z, height_types* ht, int* tiltxoff, int* tiltzoff, int* OnObject);
int GetCeiling(FLOOR_INFO* floor, int x, int y, int z);
void AlterFloorHeight(ITEM_INFO* item, int height);
void TranslateItem(ITEM_INFO* item, short x, short y, short z);
int GetChange(ITEM_INFO* item, ANIM_STRUCT* anim);
void UpdateSky();
int TriggerActive(ITEM_INFO* item);
void TriggerNormalCDTrack(short value, short flags, short type);
void TriggerCDTrack(short value, short flags, short type);
int LOS(GAME_VECTOR* start, GAME_VECTOR* target);
int ExplodeItemNode(ITEM_INFO* item, int Node, int NoXZVel, int bits);
int IsRoomOutside(int x, int y, int z);
int GetTargetOnLOS(GAME_VECTOR* src, GAME_VECTOR* dest, int DrawTarget, int firing);
void AnimateItem(ITEM_INFO* item);
int GetMaximumFloor(FLOOR_INFO* floor, int x, int z);
int GetMinimumCeiling(FLOOR_INFO* floor, int x, int z);

extern short* OutsideRoomOffsets;
extern char* OutsideRoomTable;
extern short IsRoomOutsideNo;

extern MESH_INFO* SmashedMesh[];
extern short SmashedMeshRoom[];
extern short SmashedMeshCount;

extern int flipmap[10];
extern int flip_stats[10];
extern int flip_status;
extern int flipeffect;

extern short* trigger_index;

extern int InItemControlLoop;
extern short ItemNewRooms[256][2];
extern short ItemNewRoomNo;

extern unsigned char CurrentAtmosphere;
extern unsigned char IsAtmospherePlaying;
extern char cd_flags[128];

extern unsigned int FmvSceneTriggered;
extern unsigned int CutSceneTriggered;
extern int framecount;
extern int reset_flag;
extern int WeaponDelay;
extern int LaserSightX;
extern int LaserSightY;
extern int LaserSightZ;
extern unsigned short GlobalCounter;
extern short XSoff1;
extern short XSoff2;
extern short YSoff1;
extern short YSoff2;
extern short ZSoff1;
extern short ZSoff2;
extern short FXType;
extern char PoisonFlag;
extern char TriggerTimer;
extern char LaserSightActive;
extern char DeathMenuActive;
extern SHATTER_ITEM ShatterItem;


#endif// TOMB4_GAME_CONTROL_H
