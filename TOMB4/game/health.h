#pragma once
typedef struct DISPLAYPU DISPLAYPU;

long FlashIt();
void DrawGameInfo(long timed);
void DrawHealthBar(long flash_state);
void DrawAirBar(long flash_state);
void InitialisePickUpDisplay();
void DrawPickups();
void AddDisplayPickup(short object_number);

extern DISPLAYPU pickups[];
extern long PickupX;
extern short CurrentPickup;

extern long health_bar_timer;
