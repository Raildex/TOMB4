#ifndef TOMB4_GAME_INVOBJ_H
#define TOMB4_GAME_INVOBJ_H
typedef struct INVOBJ {
	short object_number;
	short yoff;
	short scale1;
	unsigned short yrot;
	unsigned short xrot;
	unsigned short zrot;
	short flags;
	short objname;
	unsigned long meshbits;
} INVOBJ;
#endif // TOMB4_GAME_INVOBJ_H
