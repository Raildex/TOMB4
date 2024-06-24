#ifndef TOMB4_GAME_RTDECODE_H
#define TOMB4_GAME_RTDECODE_H
struct RTDECODE {
	unsigned long length;
	unsigned long off;
	unsigned short counter;
	unsigned short data;
	unsigned char decodetype;
	unsigned char packmethod;
	unsigned short padfuck;
};
#endif // TOMB4_GAME_RTDECODE_H
