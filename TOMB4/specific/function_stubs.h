#pragma once

#include <stdio.h>
#include "game/phdvector.h"

long GetRandomControl();
void SeedRandomControl(long seed);
long GetRandomDraw();
void SeedRandomDraw(long seed);
void Log(unsigned long type, const char* s, ...);

extern FILE* logF;
extern PHD_VECTOR CamPos;
extern PHD_VECTOR CamRot;
extern long nPolyType;
