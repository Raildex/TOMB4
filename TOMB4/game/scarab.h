#ifndef TOMB4_GAME_SCARAB_H
#define TOMB4_GAME_SCARAB_H
#include "game/scarabstruct.h"


void InitialiseScarab(short item_number);
void ScarabControl(short item_number);
long GetFreeScarab();
void ClearScarabs();
void TriggerScarab(short item_number);
void UpdateScarabs();
void DrawScarabs();
void InitialiseScarabGenerator(short item_number);

extern SCARAB_STRUCT Scarabs[128];


#endif// TOMB4_GAME_SCARAB_H
