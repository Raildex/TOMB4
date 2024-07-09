#pragma once
void S_GameClose();
long S_GameInitialise();
long S_SaveGame(long slot_num);
long S_LoadGame(long slot_num);
void S_GameMain();
extern long SaveCounter;
