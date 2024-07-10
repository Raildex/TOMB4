#ifndef TOMB4_GAME_HORSE_H
#define TOMB4_GAME_HORSE_H

typedef struct PHD_VECTOR PHD_VECTOR;


void InitialiseHorseman(short item_number);
void HorsemanControl(short item_number);
void TriggerHorsemanRicochets(PHD_VECTOR* pos, long yrot, long num);
void InitialiseHorse(short item_number);


#endif// TOMB4_GAME_HORSE_H
