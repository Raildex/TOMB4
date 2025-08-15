#ifndef TOMB4_SPECIFIC_FUNCTION_STUBS_H
#define TOMB4_SPECIFIC_FUNCTION_STUBS_H

#include <stdio.h>
#include "game/phdvector.h"

int GetRandomControl();
void SeedRandomControl(int seed);
int GetRandomDraw();
void SeedRandomDraw(int seed);
void Log(const char* scope, const char* s, ...);
void LogE(const char* scope, const char* s, ...);
void LogD(const char* scope, const char* s, ...);

extern FILE* logF;
extern PHD_VECTOR CamPos;
extern PHD_VECTOR CamRot;
extern int nPolyType;


#endif// TOMB4_SPECIFIC_FUNCTION_STUBS_H
