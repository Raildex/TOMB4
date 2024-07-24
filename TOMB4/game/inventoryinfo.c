#include "game/inventoryinfo.h"

void InitialiseInventory(INVENTORYINFO* inv) {
	inv->nSlots = 0;
	inv->slots = NULL;
}

void AddInventoryItem(INVENTORYINFO* inventory, enum object_types obj, unsigned short amnt) {
	long free = -1; // first slot without an amount
	for(int i = 0; i < inventory->nSlots; ++i) {
		if(free == -1 && inventory->slots[i].amount == 0) {
			free = i;
		}
		if(inventory->slots[i].object == obj) {
			inventory->slots[i].amount += amnt;
			return;
		}
	}
	if(free != -1) {
		inventory->slots[free].amount = amnt;
		inventory->slots[free].object = obj;
	} else {
		size_t idx = inventory->nSlots;
		INV_SLOT* newBuffer = (INV_SLOT*)realloc(inventory->slots, sizeof(INV_SLOT) * inventory->nSlots + 1);
		if(!newBuffer) {
			LogE(__func__, "Not enough memory to add item to inventory!");
			abort();
		}
		inventory->slots = newBuffer;
		inventory->nSlots += 1;
		inventory->slots[idx].amount = amnt;
		inventory->slots[idx].object = obj;
	}
}

void RemoveInventoryItem(INVENTORYINFO* inventory, enum object_types obj, unsigned short amnt) {
	for(int i = 0; i < inventory->nSlots; ++i) {
		if(inventory->slots[i].object == obj) {
			if(inventory->slots[i].amount - amnt >= 0) {
				inventory->slots[i].amount -= amnt;
			}else {
				inventory->slots[i].amount = 0;
			}
			break;
		}
	}
}

long HasItemInInventory(INVENTORYINFO* inventory, enum object_types obj) {
	for(int i = 0; i < inventory->nSlots; ++i) {
		if(inventory->slots[i].object == obj) {
			if(inventory->slots[i].amount >= 1) {
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

int compInventoryAmount(const void* a, const void* b) {
	INV_SLOT* slotA = (INV_SLOT*)a;
	INV_SLOT* slotB = (INV_SLOT*)b;
	if(slotA->amount > 0 && slotB->amount > 0) {
		return 0;
	}
	if(slotA->amount == 0 && slotB->amount > 0) {
		return 1;
	}
	if(slotA->amount > 0 && slotB->amount == 0) {
		return -1;
	}
	return 0;
}

void TrimInventory(INVENTORYINFO* inv) {
	qsort(inv->slots, inv->nSlots, sizeof(*inv->slots), &compInventoryAmount);
	int i;
	for(i = 0; i < inv->nSlots; ++i) {
		if(inv->slots[i].amount == 0) {
			break;
		}
	}
	INV_SLOT* newBuffer = (INV_SLOT*)realloc(inv->slots, sizeof(INV_SLOT) * (i+1));
	if(!newBuffer) {
		LogE(__func__, "Not enough memory to trim inventory!");
		abort();
	}
	inv->slots = newBuffer;
	inv->nSlots = i+1;
}