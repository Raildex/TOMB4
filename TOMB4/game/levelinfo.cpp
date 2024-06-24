#include "levelinfo.h"
#include "control.h"
#include "drawroom.h"
#include "effect2.h"
#include "function_stubs.h"
#include "objectinfo.h"
#include "objects.h"
#include "polyinsert.h"
#include "roominfo.h"
#include "setup.h"
#include "staticinfo.h"
#include <cstdlib>
#include "floorinfo.h"
#include "lightinfo.h"
#include "meshinfo.h"
#include "types.h"
LEVEL_INFO* currentLevel;
struct LEVEL_INFO {
	OBJECT_INFO* objects;
	STATIC_INFO* statics;
	long nRooms;
	ROOM_INFO* rooms;
	short* floor_data;
};

LEVEL_INFO* CreateLevel() {
	LEVEL_INFO* lvl = (LEVEL_INFO*)malloc(sizeof(LEVEL_INFO));
	lvl->objects = (OBJECT_INFO*)calloc(NUMBER_OBJECTS,sizeof(OBJECT_INFO));
	lvl->statics = (STATIC_INFO*)calloc(NUMBER_STATIC_OBJECTS, sizeof(STATIC_INFO));
	return lvl;
}

void DestroyRoom(ROOM_INFO* r) {
	free(r->data);
	free(r->door);
	//free(r->pclight);
	free(r->light);
	//free(r->FaceData);
	//free(r->vnormals);
	//free(r->verts);
	//free(r->fnormals);
	free(r->floor);
}

void DestroyLevel(LEVEL_INFO* lvl) {
	free(lvl->objects);
	free(lvl->statics);
	for(int i =0; i < lvl->nRooms; i++) {
		ROOM_INFO* r = &lvl->rooms[i];
		DestroyRoom(r);
	}
	free(lvl);
}

short* GetFloorData(LEVEL_INFO* lvl, long index) {
	return lvl->floor_data + index;
}

ROOM_INFO* GetRoom(LEVEL_INFO* lvl, long room) {
	return lvl->rooms + room;
}

OBJECT_INFO* GetObjectInfo(LEVEL_INFO* lvl, long type) {
	return lvl->objects + type;
}

STATIC_INFO* GetStaticObject(LEVEL_INFO* lvl, long type) {
	return lvl->statics + type;
}

short* GetStaticObjectBounds(LEVEL_INFO* lvl,long type) {
	return &((lvl->statics + type)->x_minc);
}

bool LoadRooms(char** FileData, LEVEL_INFO* lvl) {
	ROOM_INFO* r;
	long size, nDoors;

	Log(2, "LoadRooms");
	wibble = 0;
	NumLevelFogBulbs = 0;
	*FileData += sizeof(long);
	lvl->nRooms = *(short*)*FileData;
	*FileData += sizeof(short);
	Log(7, "Number Of Rooms %d", lvl->nRooms);

	if(lvl->nRooms < 0) {
		Log(1, "Incorrect Number Of Rooms");
		return 0;
	}

	lvl->rooms = (ROOM_INFO*)calloc(lvl->nRooms, sizeof(ROOM_INFO));

	if(!lvl->rooms)
		return 0;

	for(int i = 0; i < lvl->nRooms; i++) {
		r = GetRoom(currentLevel,i);

		r->x = *(long*)*FileData;
		*FileData += sizeof(long);

		r->y = 0;

		r->z = *(long*)*FileData;
		*FileData += sizeof(long);

		r->minfloor = *(long*)*FileData;
		*FileData += sizeof(long);

		r->maxceiling = *(long*)*FileData;
		*FileData += sizeof(long);

		size = *(long*)*FileData;
		*FileData += sizeof(long);
		r->data = (short*)malloc(size * sizeof(short));
		memcpy(r->data, *FileData, size * sizeof(short));
		*FileData += size * sizeof(short);

		nDoors = *(short*)*FileData;
		*FileData += sizeof(short);

		if(nDoors) {
			r->door = (short*)malloc((16 * nDoors + 1) * sizeof(short));
			r->door[0] = (short)nDoors;
			memcpy(r->door + 1, *FileData, 16 * nDoors * sizeof(short));
			*FileData += 16 * nDoors * sizeof(short);
		} else
			r->door = 0;

		r->x_size = *(short*)*FileData;
		*FileData += sizeof(short);

		r->y_size = *(short*)*FileData;
		*FileData += sizeof(short);

		size = r->x_size * r->y_size * sizeof(FLOOR_INFO);
		r->floor = (FLOOR_INFO*)malloc(size);
		memcpy(r->floor, *FileData, size);
		*FileData += size;

		r->ambient = *(long*)*FileData;
		*FileData += sizeof(long);

		r->num_lights = *(short*)*FileData;
		*FileData += sizeof(short);

		if(r->num_lights) {
			size = sizeof(LIGHTINFO) * r->num_lights;
			r->light = (LIGHTINFO*)malloc(size);
			memcpy(r->light, *FileData, size);
			*FileData += size;
		} else
			r->light = 0;

		r->num_meshes = *(short*)*FileData;
		*FileData += sizeof(short);

		if(r->num_meshes) {
			size = sizeof(MESH_INFO) * r->num_meshes;
			r->mesh = (MESH_INFO*)malloc(size);
			memcpy(r->mesh, *FileData, size);
			*FileData += size;

			for(int j = 0; j < r->num_meshes; j++)
				r->mesh[j].Flags = 1;
		} else
			r->mesh = 0;

		r->flipped_room = *(short*)*FileData;
		*FileData += sizeof(short);

		r->flags = *(short*)*FileData;
		*FileData += sizeof(short);

		r->MeshEffect = *(char*)*FileData;
		*FileData += sizeof(char);

		r->ReverbType = *(char*)*FileData;
		*FileData += sizeof(char);

		r->FlipNumber = *(char*)*FileData;
		*FileData += sizeof(char);

		r->left = 0x7FFF;
		r->top = 0x7FFF;
		r->bound_active = 0;
		r->right = 0;
		r->bottom = 0;
		r->item_number = NO_ITEM;
		r->fx_number = NO_ITEM;
		ProcessRoomData(r);
	}

	BuildOutsideTable(lvl);
	size = *(long*)*FileData;
	*FileData += sizeof(long);
	lvl->floor_data = (short*)malloc(2 * size);
	memcpy(lvl->floor_data, *FileData, 2 * size);
	*FileData += sizeof(short) * size;
	Log(0, "Floor Data Size %d @ %x", size, lvl->floor_data);
	return 1;
}

long GetNumRooms(LEVEL_INFO* lvl) {
	return lvl->nRooms;
}