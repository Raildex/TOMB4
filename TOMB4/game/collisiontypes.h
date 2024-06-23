#ifndef COLLISION_TYPES_INCLUDED
#define COLLISION_TYPES_INCLUDED
enum collision_types
{
	CT_NONE =			0x0,
	CT_FRONT =			0x1,
	CT_LEFT =			0x2,
	CT_RIGHT =			0x4,
	CT_TOP =			0x8,
	CT_TOP_FRONT =		0x10,
	CT_CLAMP =			0x20
};
#endif