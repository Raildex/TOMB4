#pragma once
typedef struct SAVEGAME_INFO SAVEGAME_INFO;
long CheckSumValid(char* buffer);
void sgInitialiseHub(long dont_save_lara);
void SaveLaraData();
void WriteSG(void* pointer, long size);
void ReadSG(void* pointer, long size);
void SaveHubData(long index);
void RestoreLaraData(long FullSave);
void sgRestoreLevel();
void CreateCheckSum();
void sgSaveLevel();
void sgSaveGame();
void sgRestoreGame();
long OpenSaveGame(unsigned char current_level, long saving);
void SaveLevelData(long FullSave);
void RestoreLevelData(long FullSave);

extern SAVEGAME_INFO savegame;
