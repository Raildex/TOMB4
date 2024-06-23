#ifndef DYNAMIC_INCLUDED
#define DYNAMIC_INCLUDED
struct DYNAMIC
{
	long x;
	long y;
	long z;
	unsigned char on;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned short falloff;
	unsigned char used;
	unsigned char pad1[1];
	long FalloffScale;
};
#endif