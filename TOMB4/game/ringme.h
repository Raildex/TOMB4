#ifndef TOMB4_GAME_RINGME_H
#define TOMB4_GAME_RINGME_H
#include "game/objlist.h"
struct RINGME {
	OBJLIST current_object_list[119];
	long ringactive;
	long objlistmovement;
	long curobjinlist;
	long numobjectsinlist;
};
#endif // TOMB4_GAME_RINGME_H
