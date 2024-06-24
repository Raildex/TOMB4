#ifndef SAMPLE_INFO_INCLUDED
#define SAMPLE_INFO_INCLUDED
#pragma pack(push, 1)
struct SAMPLE_INFO {
	short number;
	unsigned char volume;
	unsigned char radius;
	unsigned char randomness;
	char pitch;
	short flags;
};
#pragma pack(pop)
#endif