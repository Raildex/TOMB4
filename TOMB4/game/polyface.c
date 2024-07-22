#include "game/polyface.h"

void SetShineStrength(unsigned short* Effect, long shine) {
	if(shine > 0) {
		shine = shine & 0x1F;
		*Effect |= 1 << 1;
		*Effect |= shine << 7;
	}else {
		*Effect &= ~(1 << 1);
		*Effect &= ~(0x1F << 7);
	}
}
void SetAdditiveBlending(unsigned short* Effect, long enabled) {
	if(enabled) {
		*Effect |= 0x1;
	}else {
		*Effect &= ~0x1;
	}
}
unsigned short GetShineStrength(unsigned short Effect) {
	return (Effect >> 0x2)& 0x1F;
}

unsigned short GetAdditiveBlending(unsigned short Effect) {
	return Effect & 0x1;
}