#ifndef TOMB4_SPECIFIC_GAMEMAIN_H
#define TOMB4_SPECIFIC_GAMEMAIN_H
void S_GameClose();
int S_GameInitialise();
int S_SaveGame(int slot_num);
int S_LoadGame(int slot_num);
void S_GameMain();
void S_HandleMessages();
extern int SaveCounter;


#endif// TOMB4_SPECIFIC_GAMEMAIN_H
