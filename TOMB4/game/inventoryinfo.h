#ifndef TOMB4_GAME_INVENTORYINFO_H
#define TOMB4_GAME_INVENTORYINFO_H
#include "game/objects.h"
#include "specific/function_stubs.h"
#include <stdlib.h>
typedef struct INV_SLOT {
	enum object_types object;
	unsigned short amount;
} INV_SLOT;

typedef struct INVENTORYINFO {
	long nSlots;
	INV_SLOT* slots;
} INVENTORYINFO;

void InitialiseInventory(INVENTORYINFO* inv);

void AddInventoryItem(INVENTORYINFO* inventory, enum object_types obj, unsigned short amnt);

void RemoveInventoryItem(INVENTORYINFO* inventory, enum object_types obj, unsigned short amnt);

long HasItemInInventory(INVENTORYINFO* inventory, enum object_types obj);

void TrimInventory(INVENTORYINFO* inv);

#endif// TOMB4_GAME_INVENTORYINFO_H
