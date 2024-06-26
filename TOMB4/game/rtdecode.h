#ifndef TOMB4_GAME_RTDECODE_H
#define TOMB4_GAME_RTDECODE_H
typedef struct RTDECODE {
	unsigned long length;
	unsigned long off;
	unsigned short counter;
	unsigned short data;
	unsigned char decodetype;
	unsigned char packmethod;
	unsigned short padfuck;
} RTDECODE;
#endif // TOMB4_GAME_RTDECODE_H
