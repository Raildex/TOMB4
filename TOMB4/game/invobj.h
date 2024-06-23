#ifndef INVOBJ_INCLUDED
#define INVOBJ_INCLUDED
struct INVOBJ
{
	short object_number;
	short yoff;
	short scale1;
	unsigned short yrot;
	unsigned short xrot;
	unsigned short zrot;
	short flags;
	short objname;
	unsigned long meshbits;
};
#endif