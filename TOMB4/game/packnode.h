#ifndef TOMB4_GAME_PACKNODE_H
#define TOMB4_GAME_PACKNODE_H
#include "game/rtdecode.h"
struct PACKNODE {
	short xrot_run;
	short yrot_run;
	short zrot_run;
	short xkey;
	short ykey;
	short zkey;
	RTDECODE decode_x;
	RTDECODE decode_y;
	RTDECODE decode_z;
	unsigned long xlength;
	unsigned long ylength;
	unsigned long zlength;
	char* xpacked;
	char* ypacked;
	char* zpacked;
};
#endif // TOMB4_GAME_PACKNODE_H
