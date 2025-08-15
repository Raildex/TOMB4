#ifndef TOMB4_GAME_RINGME_H
#define TOMB4_GAME_RINGME_H
#include "game/objlist.h"
typedef struct RINGME {
	OBJLIST current_object_list[119];
	int ringactive;
	int objlistmovement;
	int curobjinlist;
	int numobjectsinlist;
} RINGME;
#endif// TOMB4_GAME_RINGME_H
