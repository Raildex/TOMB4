#ifndef RINGME_INCLUDED
#define RINGME_INCLUDED
#include "objlist.h"
struct RINGME
{
	OBJLIST current_object_list[119];
	long ringactive;
	long objlistmovement;
	long curobjinlist;
	long numobjectsinlist;
};
#endif