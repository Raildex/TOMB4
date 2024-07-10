#ifndef TOMB4_GAME_DEMIGOD_H
#define TOMB4_GAME_DEMIGOD_H

typedef struct PHD_3DPOS PHD_3DPOS;

void TriggerDemigodMissile(PHD_3DPOS* pos, short room_number, short type);
void TriggerDemigodMissileFlame(short fx_number, long xv, long yv, long zv);
void TriggerHammerSmoke(long x, long y, long z, long num);
void DoDemigodEffects(short item_number);
void InitialiseDemigod(short item_number);
void DemigodControl(short item_number);


#endif// TOMB4_GAME_DEMIGOD_H
