#ifndef TOMB4_GAME_VONCROY_H
#define TOMB4_GAME_VONCROY_H

typedef struct ITEM_INFO ITEM_INFO;
typedef struct CREATURE_INFO CREATURE_INFO;

void SetCutSceneCamera(ITEM_INFO* item);
void ClearCutSceneCamera();
void GetAIEnemy(CREATURE_INFO* info, long tfl);
void DoVonCroyCutscene(ITEM_INFO* item, CREATURE_INFO* info);
void InitialiseVoncroy(short item_number);
void VoncroyRaceControl(short item_number);
void VoncroyControl(short item_number);

extern unsigned char VonCroyCutFlags[];
extern char bVoncroyCutScene;


#endif// TOMB4_GAME_VONCROY_H
