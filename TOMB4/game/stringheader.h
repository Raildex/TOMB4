#ifndef STRINGHEADER_INCLUDED
#define STRINGHEADER_INCLUDED
struct STRINGHEADER
{
	unsigned short nStrings;
	unsigned short nPSXStrings;
	unsigned short nPCStrings;
	unsigned short StringWadLen;
	unsigned short PSXStringWadLen;
	unsigned short PCStringWadLen;
};
#endif