#ifndef TOMB4_SPECIFIC_LOAD_SAVE_H
#define TOMB4_SPECIFIC_LOAD_SAVE_H

void DoOptions();
void DisplayStatsUCunt();
int S_DisplayPauseMenu(int reset);
int DoLoadSave(int LoadSave);
int S_LoadSave(int load_or_save, int mono, int inv_active);
void S_DisplayMonoScreen();
void CreateMonoScreen();
void FreeMonoScreen();
void RGBM_Mono(unsigned char* r, unsigned char* g, unsigned char* b);
void CheckKeyConflicts();
int S_PauseMenu();
int GetSaveLoadFiles();

extern int sfx_frequencies[3];
extern int SoundQuality;
extern int MusicVolume;
extern int SFXVolume;
extern int ControlMethod;
extern char MonoScreenOn;


#endif// TOMB4_SPECIFIC_LOAD_SAVE_H
