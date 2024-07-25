#ifndef TOMB4_GAME_TRAPS_H
#define TOMB4_GAME_TRAPS_H


#include "game/heighttypes.h"
extern short SPxzoffs[8];
extern short SPyoffs[8];
typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;

void FlameEmitterControl(short item_number);
void TwoBlockPlatformFloor(ITEM_INFO* item, long x, long y, long z, long* height, height_types* height_type, long* xoff, long* zoff, long* onObject);
void TwoBlockPlatformCeiling(ITEM_INFO* item, long x, long y, long z, long* height);
void ControlTwoBlockPlatform(short item_number);
void ControlJobySpike(short item_number);
void DrawScaledSpike(ITEM_INFO* item);
void ControlSlicerDicer(short item_number);
void ControlSprinkler(short item_number);
void ControlMineHelicopter(short item_number);
void MineCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void FallingSquishyBlockCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ControlFallingSquishyBlock(short item_number);
void ControlLRSquishyBlock(short item_number);
void ControlSethBlade(short item_number);
void ControlPlinthBlade(short item_number);
void ControlMovingBlade(short item_number);
void ControlCatwalkBlade(short item_number);
void ControlBirdBlade(short item_number);
void Control4xFloorRoofBlade(short item_number);
void ControlSpikeball(short item_number);
void ControlHammer(short item_number);
void ControlStargate(short item_number);
void ControlPlough(short item_number);
void ControlChain(short item_number);
void ControlBurningFloor(short item_number);
void ControlRaisingBlock(short item_number);
void ControlScaledSpike(short item_number);
void FlameEmitter3Control(short item_number);
void FlameControl(short fx_number);
void FlameEmitter2Control(short item_number);
void LaraBurn();
void LavaBurn(ITEM_INFO* item);
long TestBoundsCollideTeethSpikes(ITEM_INFO* item);
void ControlRollingBall(short item_number);
void RollingBallCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void DartsControl(short item_number);
void DartEmitterControl(short item_number);
void FallingCeiling(short item_number);
void ControlSmashableBikeWall(short item_number);
void ControlFallingBlock2(short item_number);
void FallingBlockCeiling(ITEM_INFO* item, long x, long y, long z, long* height);
void FallingBlockFloor(ITEM_INFO* item, long x, long y, long z, long* height, height_types* height_type, long* xoff, long* zoff, long* onObject);
void FallingBlock(short item_number);
void FallingBlockCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CeilingTrapDoorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void FloorTrapDoorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CloseTrapDoor(ITEM_INFO* item);
void TrapDoorControl(short item_number);
void ControlObelisk(short item_number);

extern char LibraryTab[8];


#endif// TOMB4_GAME_TRAPS_H
