#ifndef TOMB4_GAME_POLY_H
#define TOMB4_GAME_POLY_H

typedef struct POLYFACE3 {
	unsigned short vertices[3];
	unsigned short textInfo;
	unsigned short effects;
} POLYFACE3;

typedef struct POLYFACE4 {
	unsigned short vertices[4];
	unsigned short textInfo;
	unsigned short effects;
} POLYFACE4;

void SetShineStrength(unsigned short* Effect, long shine);
void SetAdditiveBlending(unsigned short* Effect, long enabled);
unsigned short GetShineStrength(unsigned short Effect);
unsigned short GetAdditiveBlending(unsigned short Effect);
#endif// TOMB4_GAME_POLY_H
