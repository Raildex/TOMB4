#ifndef TOMB4_SPECIFIC_DRAWBARS_H
#define TOMB4_SPECIFIC_DRAWBARS_H


void S_DrawHealthBar(int pos);
void S_DrawAirBar(int pos);
void S_DrawDashBar(int pos);
void S_DrawEnemyBar(int pos);
void S_DoSlider(int x, int y, int width, int height, int pos, int c1, int c2, int c3);
void S_InitLoadBar(int maxpos);
void S_LoadBar();


#endif// TOMB4_SPECIFIC_DRAWBARS_H
