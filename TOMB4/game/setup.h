#ifndef TOMB4_GAME_SETUP_H
#define TOMB4_GAME_SETUP_H


#include "game/levelinfo.h"
void ObjectObjects();
void TrapObjects();
void BaddyObjects();
void BuildOutsideTable(LEVEL_INFO* lvl);
void reset_cutseq_vars();
void ClearFootPrints();
void InitialiseGameFlags();
void InitialiseLara();
void InitialiseObjects();
void GetAIPickups();
void GetCarriedItems();
void InitTarget();
void SetupGame();


#endif// TOMB4_GAME_SETUP_H
