#ifndef TOMB4_GAME_RTDECODE_H
#define TOMB4_GAME_RTDECODE_H
typedef struct RTDECODE {
	unsigned int length;
	unsigned int off;
	unsigned short counter;
	unsigned short data;
	unsigned char decodetype;
	unsigned char packmethod;
	unsigned short padfuck;
} RTDECODE;
#endif // TOMB4_GAME_RTDECODE_H
