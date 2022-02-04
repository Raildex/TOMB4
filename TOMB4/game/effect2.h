#pragma once
#include "../global/vars.h"

void inject_effect2(bool replace);

void ControlSmokeEmitter(short item_number);
void TriggerExplosionSmokeEnd(long x, long y, long z, long uw);
void TriggerExplosionSmoke(long x, long y, long z, long uw);

#define	ClearDynamics	( (void(__cdecl*)()) 0x00436320 )
#define KillEverything	( (void(__cdecl*)()) 0x00436050 )
#define UpdateSparks	( (void(__cdecl*)()) 0x00433E10 )
#define UpdateSplashes	( (void(__cdecl*)()) 0x004359E0 )
#define TriggerDynamic	( (void(__cdecl*)(long, long, long, long, long, long, long)) 0x004361A0 )
#define GetFreeSpark	( (long(__cdecl*)()) 0x00433D30 )
#define TriggerExplosionSparks	( (void(__cdecl*)(long, long, long, long, long, long, short)) 0x004349F0 )
#define TriggerFireFlame	( (void(__cdecl*)(long, long, long, long, long)) 0x00435130 )
