#ifndef CARRIED_WEAPON_FLAGS_INCLUDED
#define CARRIED_WEAPON_FLAGS_INCLUDED
enum carried_weapon_flags
{
	W_NONE =		0x0,
	W_PRESENT =		0x1,
	W_FLASHLIGHT =	0x2,	//speculation, actually unused
	W_LASERSIGHT =	0x4,
	W_AMMO1 =		0x8,
	W_AMMO2 =		0x10,
	W_AMMO3 =		0x20
};
#endif