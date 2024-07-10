#ifndef TOMB4_GAME_HAIR_H
#define TOMB4_GAME_HAIR_H
#include "game/hairstruct.h"
void InitialiseHair();
void HairControl(long in_cutscene, long pigtail, short* cutscenething);
void GetCorrectStashPoints(long pigtail, long hair_node, long skin_node);
void DrawHair();

extern HAIR_STRUCT hairs[2][7];


#endif// TOMB4_GAME_HAIR_H
