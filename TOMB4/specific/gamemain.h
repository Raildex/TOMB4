#ifndef TOMB4_SPECIFIC_GAMEMAIN_H
#define TOMB4_SPECIFIC_GAMEMAIN_H
void S_GameClose();
long S_GameInitialise();
long S_SaveGame(long slot_num);
long S_LoadGame(long slot_num);
void S_GameMain();
extern long SaveCounter;


#endif// TOMB4_SPECIFIC_GAMEMAIN_H
