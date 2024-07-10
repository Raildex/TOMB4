#ifndef TOMB4_GAME_SENTRYGUN_H
#define TOMB4_GAME_SENTRYGUN_H

typedef struct ITEM_INFO ITEM_INFO;


void TriggerAutogunFlamethrower(ITEM_INFO* item);
void InitialiseAutogun(short item_number);
void AutogunControl(short item_number);


#endif// TOMB4_GAME_SENTRYGUN_H
