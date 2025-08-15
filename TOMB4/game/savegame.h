#ifndef TOMB4_GAME_SAVEGAME_H
#define TOMB4_GAME_SAVEGAME_H
typedef struct SAVEGAME_INFO SAVEGAME_INFO;
int CheckSumValid(char* buffer);
void sgInitialiseHub(int dont_save_lara);
void SaveLaraData();
void WriteSG(void* pointer, int size);
void ReadSG(void* pointer, int size);
void SaveHubData(int index);
void RestoreLaraData(int FullSave);
void sgRestoreLevel();
void CreateCheckSum();
void sgSaveLevel();
void sgSaveGame();
void sgRestoreGame();
int OpenSaveGame(unsigned char current_level, int saving);
void SaveLevelData(int FullSave);
void RestoreLevelData(int FullSave);

extern SAVEGAME_INFO savegame;


#endif// TOMB4_GAME_SAVEGAME_H
