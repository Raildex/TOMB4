#ifndef TOMB4_GAME_VONCROY_H
#define TOMB4_GAME_VONCROY_H

typedef struct ITEM_INFO ITEM_INFO;
typedef struct CREATURE_INFO CREATURE_INFO;

void InitialiseVoncroy(short item_number);
void VoncroyRaceControl(short item_number);
void VoncroyControl(short item_number);

extern unsigned char VonCroyCutFlags[];
extern char bVoncroyCutScene;


#endif// TOMB4_GAME_VONCROY_H
