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
long ControlPhase(long nframes, long demo_mode);
void FlipMap(long FlipNumber);
void TestTriggers(short* data, long heavy, long HeavyFlags);
short GetDoor(FLOOR_INFO* floor);
long CheckNoColFloorTriangle(FLOOR_INFO* floor, long x, long z);
long CheckNoColCeilingTriangle(FLOOR_INFO* floor, long x, long z);
FLOOR_INFO* GetFloor(long x, long y, long z, short* room_number);
long GetWaterHeight(long x, long y, long z, short room_number);
long GetHeight(FLOOR_INFO* floor, long x, long y, long z, height_types* ht, long* tiltxoff, long* tiltzoff, long* OnObject);
long GetCeiling(FLOOR_INFO* floor, long x, long y, long z);
void AlterFloorHeight(ITEM_INFO* item, long height);
void TranslateItem(ITEM_INFO* item, short x, short y, short z);
long GetChange(ITEM_INFO* item, ANIM_STRUCT* anim);
void UpdateSky();
long TriggerActive(ITEM_INFO* item);
void TriggerNormalCDTrack(short value, short flags, short type);
void TriggerCDTrack(short value, short flags, short type);
long LOS(GAME_VECTOR* start, GAME_VECTOR* target);
long ExplodeItemNode(ITEM_INFO* item, long Node, long NoXZVel, long bits);
long IsRoomOutside(long x, long y, long z);
long GetTargetOnLOS(GAME_VECTOR* src, GAME_VECTOR* dest, long DrawTarget, long firing);
void AnimateItem(ITEM_INFO* item);
long GetMaximumFloor(FLOOR_INFO* floor, long x, long z);
long GetMinimumCeiling(FLOOR_INFO* floor, long x, long z);

extern short* OutsideRoomOffsets;
extern char* OutsideRoomTable;
extern short IsRoomOutsideNo;

extern MESH_INFO* SmashedMesh[];
extern short SmashedMeshRoom[];
extern short SmashedMeshCount;

extern long flipmap[10];
extern long flip_stats[10];
extern long flip_status;
extern long flipeffect;
extern long fliptimer;

extern short* trigger_index;

extern long InItemControlLoop;
extern short ItemNewRooms[256][2];
extern short ItemNewRoomNo;

extern unsigned char CurrentAtmosphere;
extern unsigned char IsAtmospherePlaying;
extern char cd_flags[128];

extern unsigned long FmvSceneTriggered;
extern unsigned long CutSceneTriggered;
extern long SetDebounce;
extern long framecount;
extern long reset_flag;
extern long WeaponDelay;
extern long LaserSightX;
extern long LaserSightY;
extern long LaserSightZ;
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
