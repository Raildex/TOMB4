#ifndef TOMB4_GAME_DEMIGOD_H
#define TOMB4_GAME_DEMIGOD_H

typedef struct PHD_3DPOS PHD_3DPOS;

void TriggerDemigodMissile(PHD_3DPOS* pos, short room_number, short type);
void TriggerDemigodMissileFlame(short fx_number, int xv, int yv, int zv);
void TriggerHammerSmoke(int x, int y, int z, int num);
void DoDemigodEffects(short item_number);
void InitialiseDemigod(short item_number);
void DemigodControl(short item_number);


#endif// TOMB4_GAME_DEMIGOD_H
