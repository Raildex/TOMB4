#ifndef TOMB4_GAME_TEXT_H
#define TOMB4_GAME_TEXT_H

typedef struct CHARDEF CHARDEF;


void InitFont();
void UpdatePulseColour();
int GetStringLength(const char* string, int* top, int* bottom);
void DrawChar(int x, int y, unsigned short col, CHARDEF* def);
void PrintString(int x, int y, unsigned char col, const char* string, unsigned short flags);

extern int stash_font_height;
extern int smol_font_height;
extern int small_font;
extern int font_height;
extern int GnFrameCounter;


#endif// TOMB4_GAME_TEXT_H
