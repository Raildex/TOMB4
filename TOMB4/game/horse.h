#pragma once

struct PHD_VECTOR;


void InitialiseHorseman(short item_number);
void HorsemanControl(short item_number);
void TriggerHorsemanRicochets(PHD_VECTOR* pos, long yrot, long num);
void InitialiseHorse(short item_number);
