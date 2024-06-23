#ifndef DOOR_DATA_INCLUDED
#define DOOR_DATA_INCLUDED
#include "doorposdata.h"
struct DOOR_DATA
{
	DOORPOS_DATA d1;
	DOORPOS_DATA d1flip;
	DOORPOS_DATA d2;
	DOORPOS_DATA d2flip;
	short Opened;
};
#endif