#ifndef TOMB4_SPECIFIC_FUNCTION_STUBS_H
#define TOMB4_SPECIFIC_FUNCTION_STUBS_H

#include <stdio.h>
#include "game/phdvector.h"

long GetRandomControl();
void SeedRandomControl(long seed);
long GetRandomDraw();
void SeedRandomDraw(long seed);
void Log(const char* scope, const char* s, ...);
void LogE(const char* scope, const char* s, ...);
void LogD(const char* scope, const char* s, ...);

extern FILE* logF;
extern PHD_VECTOR CamPos;
extern PHD_VECTOR CamRot;
extern long nPolyType;


#endif// TOMB4_SPECIFIC_FUNCTION_STUBS_H
