#ifndef RTDECODE_INCLUDED
#define RTDECODE_INCLUDED
struct RTDECODE
{
	unsigned long length;
	unsigned long off;
	unsigned short counter;
	unsigned short data;
	unsigned char decodetype;
	unsigned char packmethod;
	unsigned short padfuck;
};
#endif