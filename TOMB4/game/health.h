#ifndef TOMB4_GAME_HEALTH_H
#define TOMB4_GAME_HEALTH_H
#include "game/displaypu.h"

int FlashIt();
void DrawGameInfo(int timed);
void DrawHealthBar(int flash_state);
void DrawAirBar(int flash_state);
void InitialisePickUpDisplay();
void DrawPickups();
void AddDisplayPickup(short object_number);

extern DISPLAYPU pickups[8];
extern int PickupX;
extern short CurrentPickup;

extern int health_bar_timer;


#endif// TOMB4_GAME_HEALTH_H
