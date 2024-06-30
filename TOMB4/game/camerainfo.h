#ifndef TOMB4_GAME_CAMERAINFO_H
#define TOMB4_GAME_CAMERAINFO_H
#include "game/gamevector.h"
#include "game/cameratype.h"
#include "game/phdvector.h"
typedef struct ITEM_INFO ITEM_INFO;
typedef struct OBJECT_VECTOR OBJECT_VECTOR;

typedef struct CAMERA_INFO {
	GAME_VECTOR pos;
	GAME_VECTOR target;
	camera_type type;
	camera_type old_type;
	long shift;
	long flags;
	long fixed_camera;
	long number_frames;
	long bounce;
	long underwater;
	long target_distance;
	short target_angle;
	short target_elevation;
	short actual_elevation;
	short actual_angle;
	short node;
	short number;
	short last;
	short timer;
	short speed;
	ITEM_INFO* item;
	ITEM_INFO* last_item;
	long mike_at_lara;
	PHD_VECTOR mike_pos;
} CAMERA_INFO;
#endif // TOMB4_GAME_CAMERAINFO_H
