#ifndef TOMB4_GAME_SAVEGAMEINFO_H
#define TOMB4_GAME_SAVEGAMEINFO_H
#include "stats.h"
#include "cvector.h"
#include "larainfo.h"
#pragma pack(push,1)
struct SAVEGAME_INFO
{
	LARA_INFO Lara;
	long cutscene_triggered;
	unsigned char HubLevels[10];	//saved level indices. highest one that isn't 0 is the one we are currently in
	unsigned short HubOffsets[10];	//offset of each level's data inside the savegame buffer
	unsigned short HubSizes[10];	//size of each level's data inside the savegame buffer
	char CurrentLevel;
	char Checksum;
	STATS Game;
	STATS Level;
	short WeaponObject;
	short WeaponAnim;
	short WeaponFrame;
	short WeaponCurrent;
	short WeaponGoal;
	CVECTOR fog_colour;
	unsigned char HubSavedLara : 1;	//flag that we saved Lara's data when we initialised hub, only set to 1 when InitialiseHub is called with 1
	unsigned char AutoTarget : 1;
	unsigned char HaveBikeBooster : 1;	//have the bike nitro thing
	char buffer[15410];
};
#pragma pack(pop)
#endif// TOMB4_GAME_SAVEGAMEINFO_H
