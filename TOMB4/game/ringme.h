#ifndef TOMB4_GAME_RINGME_H
#define TOMB4_GAME_RINGME_H
#include "game/objlist.h"
typedef struct RINGME {
	OBJLIST current_object_list[119];
	long ringactive;
	long objlistmovement;
	long curobjinlist;
	long numobjectsinlist;
} RINGME;
#endif// TOMB4_GAME_RINGME_H
