#ifndef TOMB4_GAME_HAIR_H
#define TOMB4_GAME_HAIR_H
#include "game/hairstruct.h"
void InitialiseHair();
void HairControl(int in_cutscene, int pigtail, short* cutscenething);
void GetCorrectStashPoints(int pigtail, int hair_node, int skin_node);
void DrawHair();

extern HAIR_STRUCT hairs[2][7];


#endif// TOMB4_GAME_HAIR_H
