#ifndef TOMB4_GAME_STRINGHEADER_H
#define TOMB4_GAME_STRINGHEADER_H
typedef struct STRINGHEADER {
	unsigned short nStrings;
	unsigned short nPSXStrings;
	unsigned short nPCStrings;
	unsigned short StringWadLen;
	unsigned short PSXStringWadLen;
	unsigned short PCStringWadLen;
} STRINGHEADER;
#endif // TOMB4_GAME_STRINGHEADER_H
