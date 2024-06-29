#include "game/levelinfo.h"
#include "game/animstruct.h"
#include "game/changestruct.h"
#include "game/control.h"
#include "game/languages.h"
#include "levelinfo.h"
#include "specific/drawroom.h"
#include "game/effect2.h"
#include "specific/function_stubs.h"
#include "game/fxinfo.h"
#include "game/gameflow.h"
#include "game/gfleveloptions.h"
#include "game/iteminfo.h"
#include "game/items.h"
#include "game/laraskin.h"
#include "game/meshdata.h"
#include "game/objectinfo.h"
#include "game/objects.h"
#include "specific/haltexture.h"
#include "specific/polyinsert.h"
#include "game/rangestruct.h"
#include "game/roominfo.h"
#include "game/samplebuffer.h"
#include "game/sampleinfo.h"
#include "game/setup.h"
#include "game/staticinfo.h"
#include <assert.h>
#include <crtdbg.h>
#include <string.h>
#include <stdio.h>
#include <ddraw.h>
#include <stdlib.h>
#include <winnt.h>
#include "game/floorinfo.h"
#include "game/lightinfo.h"
#include "game/meshinfo.h"
#include "global/types.h"
#include "specific/texture.h"
#include "specific/winmain.h"
#include "game/boxinfo.h"
#include "specific/dxsound.h"
#include "game/texturestruct.h"
#include "game/phdtexturestruct.h"
#include "specific/file.h"

enum num_samples {
	sample_lookuptable_size = 512,
};

LEVEL_INFO* currentLevel;
struct LEVEL_INFO {
	OBJECT_INFO* objects;
	STATIC_INFO* statics;
	long nRooms;
	ROOM_INFO* rooms;
	short* floor_data;
	short* mesh_base;
	short** meshes;
	ANIM_STRUCT* anims;
	RANGE_STRUCT* ranges;
	CHANGE_STRUCT* changes;
	short* commands;
	long* bones;
	short* frames;
	FX_INFO* effects;
	long effectsCapacity;
	ITEM_INFO* items;
	long level_items; // how many items were stored in the level file
	long itemsCapacity; // how many ITEM_INFOs are within items
	BOX_INFO* boxes;
	unsigned short* overlaps;
	short* ground_zone[5][2];
	long num_boxes;
	long num_samples;
	SAMPLE_BUFFER* sample_buffers;
	short* sample_lut;
	long num_sample_infos;
	SAMPLE_INFO* sample_infos;
	long nTextures;
	TEXTURE* Textures;
	TEXTURESTRUCT* textinfo;
};

LEVEL_INFO* CreateLevel() {
	LEVEL_INFO* lvl = (LEVEL_INFO*)calloc(1, sizeof(LEVEL_INFO));
	lvl->objects = (OBJECT_INFO*)calloc(NUMBER_OBJECTS, sizeof(OBJECT_INFO));
	lvl->statics = (STATIC_INFO*)calloc(NUMBER_STATIC_OBJECTS, sizeof(STATIC_INFO));
	lvl->effects = (FX_INFO*)calloc(128, sizeof(FX_INFO));
	lvl->effectsCapacity = 128;
	lvl->items = (ITEM_INFO*)calloc(1024, sizeof(ITEM_INFO));
	lvl->itemsCapacity = 1024;
	lvl->Textures = (TEXTURE*)calloc(1,sizeof(TEXTURE));
	lvl->nTextures = 1;
	return lvl;
}

void ProcessMeshData(LEVEL_INFO* lvl,long num_meshes);

char LoadObjects(char** data, LEVEL_INFO* lvl) {
	OBJECT_INFO* obj;
	STATIC_INFO* stat;
	short** mesh;
	short** mesh_size;
	long size, num, slot;
	static long num_meshes, num_anims;

	size = *(long*)*data;
	*data += sizeof(long);
	lvl->mesh_base = (short*)calloc(size, sizeof(short));
	memcpy(lvl->mesh_base, *data, size * sizeof(short));
	*data += size * sizeof(short);

	size = *(long*)*data;
	*data += sizeof(long);
	lvl->meshes = (short**)calloc(2 * size, sizeof(short*));
	memcpy(lvl->meshes, *data, size * sizeof(short*));
	*data += size * sizeof(short*);

	for(int i = 0; i < size; i++)
		lvl->meshes[i] = lvl->mesh_base + (long)(uintptr_t)(lvl->meshes[i]) / 2;

	num_meshes = size;

	num_anims = *(long*)*data;
	*data += sizeof(long);
	lvl->anims = (ANIM_STRUCT*)calloc(num_anims, sizeof(ANIM_STRUCT));
	memcpy(lvl->anims, *data, sizeof(ANIM_STRUCT) * num_anims);
	*data += sizeof(ANIM_STRUCT) * num_anims;

	size = *(long*)*data;
	*data += sizeof(long);
	lvl->changes = (CHANGE_STRUCT*)calloc(size, sizeof(CHANGE_STRUCT));
	memcpy(lvl->changes, *data, sizeof(CHANGE_STRUCT) * size);
	*data += sizeof(CHANGE_STRUCT) * size;

	size = *(long*)*data;
	*data += sizeof(long);
	lvl->ranges = (RANGE_STRUCT*)calloc(size, sizeof(RANGE_STRUCT));
	memcpy(lvl->ranges, *data, sizeof(RANGE_STRUCT) * size);
	*data += sizeof(RANGE_STRUCT) * size;

	size = *(long*)*data;
	*data += sizeof(long);
	lvl->commands = (short*)calloc(size, sizeof(short));
	memcpy(lvl->commands, *data, sizeof(short) * size);
	*data += sizeof(short) * size;

	size = *(long*)*data;
	*data += sizeof(long);
	lvl->bones = (long*)calloc(size, sizeof(long));
	memcpy(lvl->bones, *data, sizeof(long) * size);
	*data += sizeof(long) * size;

	size = *(long*)*data;
	*data += sizeof(long);
	lvl->frames = (short*)calloc(size, sizeof(short));
	memcpy(lvl->frames, *data, sizeof(short) * size);
	*data += sizeof(short) * size;

	for(int i = 0; i < num_anims; i++)
		lvl->anims[i].frame_ptr = (short*)((long)lvl->anims[i].frame_ptr + (long)lvl->frames);

	num = *(long*)*data;
	*data += sizeof(long);

	for(int i = 0; i < num; i++) {
		slot = *(long*)*data;
		*data += sizeof(long);
		obj = GetObjectInfo(currentLevel, slot);

		obj->nmeshes = *(short*)*data;
		*data += sizeof(short);

		obj->mesh_index = *(short*)*data;
		*data += sizeof(short);

		obj->bone_index = *(long*)*data;
		*data += sizeof(long);

		obj->frame_base = (short*)(*(short**)*data);
		*data += sizeof(short*);

		obj->anim_index = *(short*)*data;
		*data += sizeof(short);

		obj->loaded = 1;
	}

	CreateSkinningData();

	for(int i = 0; i < NUMBER_OBJECTS; i++) {
		obj = GetObjectInfo(currentLevel, i);
		obj->mesh_index *= 2;
	}

	mesh = lvl->meshes;
	mesh_size = &lvl->meshes[num_meshes];
	memcpy(mesh_size, mesh, num_meshes * 4);

	for(int i = 0; i < num_meshes; i++) {
		*mesh++ = *mesh_size;
		*mesh++ = *mesh_size;
		mesh_size++;
	}

	InitialiseObjects();

	num = *(long*)*data; // statics
	*data += sizeof(long);

	for(int i = 0; i < num; i++) {
		slot = *(long*)*data;
		*data += sizeof(long);
		stat = GetStaticObject(currentLevel, slot);

		stat->mesh_number = *(short*)*data;
		*data += sizeof(short);

		memcpy(&stat->x_minp, *data, 6 * sizeof(short));
		*data += 6 * sizeof(short);

		memcpy(&stat->x_minc, *data, 6 * sizeof(short));
		*data += 6 * sizeof(short);

		stat->flags = *(short*)*data;
		*data += sizeof(short);
	}

	for(int i = 0; i < NUMBER_STATIC_OBJECTS; i++) {
		stat = GetStaticObject(currentLevel, i);
		stat->mesh_number *= 2;
	}

	Log(2, "ProcessMeshData %d", num_meshes);
	num_level_meshes = num_meshes;
	mesh_vtxbuf = (MESH_DATA**)malloc(4 * num_meshes);
	lvl->mesh_base = (short*)mesh_vtxbuf;
	short* last_mesh_ptr = 0;
	MESH_DATA* msh = (MESH_DATA*)(uintptr_t)num_meshes;
	ProcessMeshData(lvl, num_meshes * 2);
	return 1;
}

void DestroyRoom(ROOM_INFO* r) {
	free(r->data);
	free(r->door);
	// free(r->pclight);
	free(r->light);
	// free(r->FaceData);
	// free(r->vnormals);
	// free(r->verts);
	// free(r->fnormals);
	free(r->floor);
}

void DestroyLevel(LEVEL_INFO* lvl) {
	free(lvl->objects);
	free(lvl->statics);
	for(int i = 0; i < lvl->nRooms; i++) {
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

short* GetStaticObjectBounds(LEVEL_INFO* lvl, long type) {
	return &((lvl->statics + type)->x_minc);
}

short** GetMeshPointer(LEVEL_INFO* lvl, long mesh) {
	return lvl->meshes + mesh;
}

short* GetMesh(LEVEL_INFO* lvl, long mesh) {
	return lvl->meshes[mesh];
}
short* GetMeshBase(LEVEL_INFO* lvl) {
	return lvl->mesh_base;
}

char LoadRooms(char** data, LEVEL_INFO* lvl) {
	ROOM_INFO* r;
	long size, nDoors;

	Log(2, "LoadRooms");
	wibble = 0;
	NumLevelFogBulbs = 0;
	*data += sizeof(long);
	lvl->nRooms = *(short*)*data;
	*data += sizeof(short);
	Log(7, "Number Of Rooms %d", lvl->nRooms);

	if(lvl->nRooms < 0) {
		Log(1, "Incorrect Number Of Rooms");
		return 0;
	}

	lvl->rooms = (ROOM_INFO*)calloc(lvl->nRooms, sizeof(ROOM_INFO));

	if(!lvl->rooms)
		return 0;

	for(int i = 0; i < lvl->nRooms; i++) {
		r = GetRoom(currentLevel, i);

		r->x = *(long*)*data;
		*data += sizeof(long);

		r->y = 0;

		r->z = *(long*)*data;
		*data += sizeof(long);

		r->minfloor = *(long*)*data;
		*data += sizeof(long);

		r->maxceiling = *(long*)*data;
		*data += sizeof(long);

		size = *(long*)*data;
		*data += sizeof(long);
		r->data = (short*)malloc(size * sizeof(short));
		memcpy(r->data, *data, size * sizeof(short));
		*data += size * sizeof(short);

		nDoors = *(short*)*data;
		*data += sizeof(short);

		if(nDoors) {
			r->door = (short*)malloc((16 * nDoors + 1) * sizeof(short));
			r->door[0] = (short)nDoors;
			memcpy(r->door + 1, *data, 16 * nDoors * sizeof(short));
			*data += 16 * nDoors * sizeof(short);
		} else
			r->door = 0;

		r->x_size = *(short*)*data;
		*data += sizeof(short);

		r->y_size = *(short*)*data;
		*data += sizeof(short);

		size = r->x_size * r->y_size * sizeof(FLOOR_INFO);
		r->floor = (FLOOR_INFO*)malloc(size);
		memcpy(r->floor, *data, size);
		*data += size;

		r->ambient = *(long*)*data;
		*data += sizeof(long);

		r->num_lights = *(short*)*data;
		*data += sizeof(short);

		if(r->num_lights) {
			size = sizeof(LIGHTINFO) * r->num_lights;
			r->light = (LIGHTINFO*)malloc(size);
			memcpy(r->light, *data, size);
			*data += size;
		} else
			r->light = 0;

		r->num_meshes = *(short*)*data;
		*data += sizeof(short);

		if(r->num_meshes) {
			size = sizeof(MESH_INFO) * r->num_meshes;
			r->mesh = (MESH_INFO*)malloc(size);
			memcpy(r->mesh, *data, size);
			*data += size;

			for(int j = 0; j < r->num_meshes; j++)
				r->mesh[j].Flags = 1;
		} else
			r->mesh = 0;

		r->flipped_room = *(short*)*data;
		*data += sizeof(short);

		r->flags = *(short*)*data;
		*data += sizeof(short);

		r->MeshEffect = *(char*)*data;
		*data += sizeof(char);

		r->ReverbType = *(char*)*data;
		*data += sizeof(char);

		r->FlipNumber = *(char*)*data;
		*data += sizeof(char);

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
	size = *(long*)*data;
	*data += sizeof(long);
	lvl->floor_data = (short*)malloc(2 * size);
	memcpy(lvl->floor_data, *data, 2 * size);
	*data += sizeof(short) * size;
	Log(0, "Floor Data Size %d @ %x", size, lvl->floor_data);
	return 1;
}

void ProcessMeshData(LEVEL_INFO* lvl, long num_meshes) {
	MESH_DATA* mesh;
	short* mesh_ptr;
	short* last_mesh_ptr;

	Log(2, "ProcessMeshData %d", num_meshes);
	num_level_meshes = num_meshes;
	mesh_vtxbuf = (MESH_DATA**)malloc(4 * num_meshes);
	lvl->mesh_base = (short*)mesh_vtxbuf;
	last_mesh_ptr = 0;
	mesh = (MESH_DATA*)(uintptr_t)num_meshes;

	for(int i = 0; i < num_meshes; i++) {
		mesh_ptr = GetMesh(currentLevel, i);

		ProcessMesh(lvl,mesh_ptr,last_mesh_ptr,i);
	}

	Log(2, "End ProcessMeshData");
}


long GetNumRooms(LEVEL_INFO* lvl) {
	return lvl->nRooms;
}

ANIM_STRUCT* GetAnim(LEVEL_INFO* lvl, long anim) {
	return lvl->anims + anim;
}

CHANGE_STRUCT* GetAnimChange(LEVEL_INFO* lvl, long index) {
	return lvl->changes + index;
}
RANGE_STRUCT* GetAnimRange(LEVEL_INFO* lvl, long index) {
	return lvl->ranges + index;
}

short* GetAnimFrames(LEVEL_INFO* lvl, long offset) {
	return lvl->frames + offset;
}

short* GetAnimFrameBase(LEVEL_INFO* lvl) {
	return lvl->frames;
}

short* GetAnimCommand(LEVEL_INFO* lvl, long index) {
	return lvl->commands + index;
}

long* GetBone(LEVEL_INFO* lvl, long index) {
	return lvl->bones + index;
}

FX_INFO* GetEffect(LEVEL_INFO* lvl, long fx) {
	return lvl->effects + fx;
}

char LoadItems(char** data, LEVEL_INFO* lvl) {
	ITEM_INFO* item;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	STATIC_INFO* stat;

	long num_items, x, y, z;

	Log(2, "LoadItems");
	num_items = *(long*)*data;
	*data += 4;

	if(!num_items)
		return 1;

	lvl->level_items = num_items;
	InitialiseItemArray((short)lvl->itemsCapacity, lvl->level_items);

	for(int i = 0; i < num_items; i++) {
		item = GetItem(lvl, i);

		item->object_number = *(short*)*data;
		*data += sizeof(short);

		item->room_number = *(short*)*data;
		*data += sizeof(short);

		item->pos.x_pos = *(long*)*data;
		*data += sizeof(long);

		item->pos.y_pos = *(long*)*data;
		*data += sizeof(long);

		item->pos.z_pos = *(long*)*data;
		*data += sizeof(long);

		item->pos.y_rot = *(short*)*data;
		*data += sizeof(short);

		item->shade = *(short*)*data;
		*data += sizeof(short);

		item->trigger_flags = *(short*)*data;
		*data += sizeof(short);

		item->flags = *(short*)*data;
		*data += sizeof(short);
	}

	for(int i = 0; i < GetNumLevelItems(lvl); i++)
		InitialiseItem(i);

	for(int i = 0; i < GetNumRooms(currentLevel); i++) {
		r = GetRoom(currentLevel, i);

		for(int j = 0; j < r->num_meshes; j++) {
			x = (r->mesh[j].x - r->x) >> 10;
			z = (r->mesh[j].z - r->z) >> 10;

			floor = &(r->floor[x * r->x_size + z]);

			if(!(lvl->boxes[floor->box].overlap_index & 0x4000)) {
				stat = GetStaticObject(currentLevel, r->mesh[j].static_number);
				y = floor->floor << 8;

				if(y <= (r->mesh[j].y - stat->y_maxc + 512) && y < r->mesh[j].y - stat->y_minc) {
					if(!stat->x_maxc || !stat->x_minc || !stat->z_maxc || !stat->z_minc || ((stat->x_maxc ^ stat->x_minc) & 0x8000 && (stat->z_maxc ^ stat->z_minc) & 0x8000)) {
						x = (r->mesh[j].x - r->x) >> 10;
						z = (r->mesh[j].z - r->z) >> 10;
						r->floor[x * r->x_size + z].stopper = 1;
					}
				}
			}
		}
	}

	return 1;
}

short* GetZone(LEVEL_INFO* lvl, long zone, long flip) {
	return lvl->ground_zone[zone][flip];
}

short GetItemNum(LEVEL_INFO* lvl, ITEM_INFO* i) {
	return i - lvl->items;
}

long GetNumBoxes(LEVEL_INFO* lvl) {
	return lvl->num_boxes;
}

char LoadBoxes(char** data, LEVEL_INFO* lvl) {
	BOX_INFO* box;
	long size;

	Log(2, "LoadBoxes");
	lvl->num_boxes = *(long*)*data;
	*data += sizeof(long);

	lvl->boxes = (BOX_INFO*)calloc(lvl->num_boxes, sizeof(BOX_INFO));
	memcpy(lvl->boxes, *data, sizeof(BOX_INFO) * lvl->num_boxes);
	*data += sizeof(BOX_INFO) * lvl->num_boxes;

	size = *(long*)*data;
	*data += sizeof(long);
	lvl->overlaps = (unsigned short*)calloc(size, sizeof(unsigned short));
	memcpy(lvl->overlaps, *data, sizeof(unsigned short) * size);
	*data += sizeof(unsigned short) * size;

	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 4; j++) {
			lvl->ground_zone[j][i] = (short*)calloc(lvl->num_boxes, sizeof(short));
			memcpy(lvl->ground_zone[j][i], *data, sizeof(short) * lvl->num_boxes);
			*data += sizeof(short) * lvl->num_boxes;
		}

		lvl->ground_zone[4][i] = (short*)calloc(lvl->num_boxes, sizeof(short));
		memcpy(lvl->ground_zone[4][i], *data, sizeof(short) * lvl->num_boxes);
		*data += sizeof(short) * lvl->num_boxes;
	}

	for(int i = 0; i < lvl->num_boxes; i++) {
		box = &lvl->boxes[i];

		if(box->overlap_index & 0x8000)
			box->overlap_index |= 0x4000;
		else if(gfLevelFlags & GF_TRAIN && box->height > -256)
			box->overlap_index |= 0xC000;
	}

	return 1;
}

ITEM_INFO* GetItem(LEVEL_INFO* lvl, long item) {
	if(item == NO_ITEM)
		return NULL;
	return lvl->items + item;
}

long GetNumLevelItems(LEVEL_INFO* lvl) {
	return lvl->level_items;
}

BOX_INFO* GetBox(LEVEL_INFO* lvl, long box) {
	return lvl->boxes + box;
}

unsigned short* GetOverlap(LEVEL_INFO* lvl, long overlap) {
	return lvl->overlaps + overlap;
}

char LoadSamples(FILE* file, char** data, LEVEL_INFO* lvl) {
	long uncomp_size, comp_size;

	Log(2, "LoadSamples");
	lvl->sample_lut = (short*)calloc(sample_lookuptable_size, sizeof(short));
	memcpy(lvl->sample_lut, *data, sample_lookuptable_size * sizeof(short));
	*data += sample_lookuptable_size * sizeof(short);
	lvl->num_sample_infos = *(long*)*data;
	*data += sizeof(long);
	Log(8, "Number Of Sample Infos %d", lvl->num_sample_infos);

	if(!lvl->num_sample_infos) {
		Log(1, "No Sample Infos");
		return 0;
	}

	lvl->sample_infos = (SAMPLE_INFO*)calloc(lvl->num_sample_infos, sizeof(SAMPLE_INFO));
	memcpy(lvl->sample_infos, *data, sizeof(SAMPLE_INFO) * lvl->num_sample_infos);
	*data += sizeof(SAMPLE_INFO) * lvl->num_sample_infos;
	lvl->num_samples = *(long*)*data;
	*data += sizeof(long);

	if(!lvl->num_samples) {
		Log(1, "No Samples");
		return 0;
	}

	Log(8, "Number Of Samples %d", lvl->num_samples);
	lvl->sample_buffers = (SAMPLE_BUFFER*)calloc(lvl->num_samples, sizeof(SAMPLE_BUFFER));

	fread(&lvl->num_samples, 1, 4, file);

	if(lvl->num_samples <= 0) {
		return 1;
	}

	for(int i = 0; i < lvl->num_samples; i++) {
		unsigned char* decompression_buffer = (unsigned char*)calloc(0xF1000, sizeof(unsigned char));
		fread(&uncomp_size, 1, 4, file);
		fread(&comp_size, 1, 4, file);
		fread(decompression_buffer, comp_size, 1, file);

		if(!S_ConvertSamples(decompression_buffer, comp_size, uncomp_size, i, lvl->sample_buffers)) {
			free(decompression_buffer);
			return 0;
		}
		free(decompression_buffer);
	}
	return 1;
}

SAMPLE_BUFFER* GetSampleBuffer(LEVEL_INFO* lvl, long num) {
	return lvl->sample_buffers + num;
}

SAMPLE_INFO* GetSampleInfo(LEVEL_INFO* lvl, long num) {
	return lvl->sample_infos + num;
}

short* GetSampleLookup(LEVEL_INFO* lvl, long num) {
	return lvl->sample_lut + num;
}

void AdjustUV(long num, LEVEL_INFO* lvl) {
	TEXTURESTRUCT* tex;
	float u, v;
	unsigned short type;

	Log(2, "AdjustUV");

	for(int i = 0; i < num; i++) {
		tex = &lvl->textinfo[i];
		lvl->Textures[tex->tpage].tpage++;
		tex->tpage++;
		u = 1.0F / (float)(lvl->Textures[tex->tpage].width << 1);
		v = 1.0F / (float)(lvl->Textures[tex->tpage].height << 1);
		type = tex->flag & 7;

		if(tex->flag & 0x8000) {
			switch(type) {
			case 0:
				tex->u1 += u;
				tex->v1 += v;
				tex->u2 -= u;
				tex->v2 += v;
				tex->u3 += u;
				tex->v3 -= v;
				break;

			case 1:
				tex->u1 -= u;
				tex->v1 += v;
				tex->u2 -= u;
				tex->v2 -= v;
				tex->u3 += u;
				tex->v3 += v;
				break;

			case 2:
				tex->u1 -= u;
				tex->v1 -= v;
				tex->u2 += u;
				tex->v2 -= v;
				tex->u3 -= u;
				tex->v3 += v;
				break;

			case 3:
				tex->u1 += u;
				tex->v1 -= v;
				tex->u2 += u;
				tex->v2 += v;
				tex->u3 -= u;
				tex->v3 -= v;
				break;

			case 4:
				tex->u1 -= u;
				tex->v1 += v;
				tex->u2 += u;
				tex->v2 += v;
				tex->u3 -= u;
				tex->v3 -= v;
				break;

			case 5:
				tex->u1 += u;
				tex->v1 += v;
				tex->u2 += u;
				tex->v2 -= v;
				tex->u3 -= u;
				tex->v3 += v;
				break;

			case 6:
				tex->u1 += u;
				tex->v1 -= v;
				tex->u2 -= u;
				tex->v2 -= v;
				tex->u3 += u;
				tex->v3 += v;
				break;

			case 7:
				tex->u1 -= u;
				tex->v1 -= v;
				tex->u2 -= u;
				tex->v2 += v;
				tex->u3 += u;
				tex->v3 -= v;
				break;

			default:
				Log(1, "TextureInfo Type %d Not Found", type);
				break;
			}
		} else {
			switch(type) {
			case 0:
				tex->u1 += u;
				tex->v1 += v;
				tex->u2 -= u;
				tex->v2 += v;
				tex->u3 -= u;
				tex->v3 -= v;
				tex->u4 += u;
				tex->v4 -= v;
				break;

			case 1:
				tex->u1 -= u;
				tex->v1 += v;
				tex->u2 += u;
				tex->v2 += v;
				tex->u3 += u;
				tex->v3 -= v;
				tex->u4 -= u;
				tex->v4 -= v;
				break;

			default:
				Log(1, "TextureInfo Type %d Not Found", type);
				break;
			}
		}
	}
}

char LoadTextureInfos(char** data, LEVEL_INFO* lvl) {
	TEXTURESTRUCT* t;
	PHDTEXTURESTRUCT tex;
	long val;

	Log(2, "LoadTextureInfos");
	*data += 3;

	val = *(long*)*data;
	*data += sizeof(long);
	Log(5, "Texture Infos : %d", val);
	lvl->textinfo = (TEXTURESTRUCT*)calloc(val, sizeof(TEXTURESTRUCT));

	for(int i = 0; i < val; i++) {
		t = &lvl->textinfo[i];
		memcpy(&tex, *data, sizeof(PHDTEXTURESTRUCT));
		*data += sizeof(PHDTEXTURESTRUCT);
		t->drawtype = tex.drawtype;
		t->tpage = tex.tpage & 0x7FFF;
		t->flag = tex.tpage ^ ((tex.tpage ^ tex.flag) & 0x7FFF);
		t->u1 = (float)(tex.u1) * (1.0F / 65535.0F);
		t->v1 = (float)(tex.v1) * (1.0f / 65535.0F);
		t->u2 = (float)(tex.u2) * (1.0f / 65535.0F);
		t->v2 = (float)(tex.v2) * (1.0f / 65535.0F);
		t->u3 = (float)(tex.u3) * (1.0f / 65535.0F);
		t->v3 = (float)(tex.v3) * (1.0f / 65535.0F);
		t->u4 = (float)(tex.u4) * (1.0f / 65535.0F);
		t->v4 = (float)(tex.v4) * (1.0f / 65535.0F);
	}

	AdjustUV(val,lvl);
	return 1;
}

char LoadTextures(TEXTURE_FORMAT fmt, FILE* f, LEVEL_INFO* lvl) {
	unsigned short BTPages;
	unsigned short RTPages;
	unsigned short OTPages;
	fread(&RTPages, sizeof(RTPages), 1, f);
	fread(&OTPages, sizeof(OTPages), 1, f);
	fread(&BTPages, sizeof(BTPages), 1, f);
	Log(1,"RTPages %d OTPages %d BTPages %d",RTPages,OTPages,BTPages);
	char texMarker[3];
	fread(texMarker, 1, 3, f);
	if(!(texMarker[0] == 'T' && texMarker[1] == 'E' && texMarker[2] == 'X')) {
		Log(-1, "Invalid Marker! %c %c %c", texMarker[0],texMarker[1],texMarker[2]);
		return 0;
	}
	Log(-1, "Marker %c %c %c", texMarker[0],texMarker[1],texMarker[2]);
	unsigned long uncompSize;
	unsigned long compSize;
	fread(&uncompSize, 4, 1, f);
	fread(&compSize, 4, 1, f);
	char* Compressed32Data = (char*)calloc(1, compSize);
	char* Uncompressed32Data = (char*)calloc(1, uncompSize);
	fread(Compressed32Data, compSize, 1, f);
	if(!S_Decompress(Uncompressed32Data, Compressed32Data, compSize, uncompSize)) {
		Log(-1, "Could not decompress texture data");
		free(Compressed32Data);
		free(Uncompressed32Data);
		return 0;
	}
	free(Compressed32Data);
	Log(1,"Successfully decompressed Texture Data!");
	Log(1, "Allocating Texture Space");
	short count = RTPages + OTPages + BTPages + 2+1;
	if(gfCurrentLevel == 0) {
		Log(1, "Current Level is Title, Allocate Space for Logo as well");
		count += 2;
	}
	Log(1, "Total count of texture pages: %d",count);
	if(lvl->Textures) {
		lvl->Textures = realloc(lvl->Textures, (count+1) * sizeof(TEXTURE));
		memset(lvl->Textures, 0, (1+count) * sizeof(TEXTURE));
	}else {
		lvl->Textures = calloc(count+1, sizeof(TEXTURE));
	}
	lvl->nTextures = count;
	for (int i = 0; i < RTPages + OTPages; ++i) {
		long* src = (long*)Uncompressed32Data + i * ((256 * 256));
		TEXTURE* tex = &lvl->Textures[i+1];
		tex->width = (unsigned short)App.TextureSize;
		tex->height = (unsigned short)App.TextureSize;
		if(!CreateTexturePage(tex->width, tex->height, fmt,b8g8r8a8, CalcMipMapCount(tex->width,tex->height), src, NULL, &tex->hal) ) {
			Log(-1, "Could not create texture page for texture %d",i);
			free(Uncompressed32Data);
			return 0;
		}
	}
	Log(1, "Create bump maps");
	long offset = RTPages + OTPages;
	for (int i = 0; i < BTPages / 2; ++i) {
		long* src = (long*)Uncompressed32Data + (i + offset) * ((256 * 256));
		TEXTURE* tex = &lvl->Textures[offset + i + 1];
		tex->width = (unsigned short)App.TextureSize;
		tex->height = (unsigned short)App.TextureSize;
		tex->bumptpage = offset + i + (BTPages / 2) + 1;
		tex->bump = 1;
		if(!CreateTexturePage(tex->width, tex->height, fmt,b8g8r8a8, CalcMipMapCount(tex->width,tex->height), src, NULL, &tex->hal) ) {
			Log(-1, "Could not create texture page for texture %d",i);
			free(Uncompressed32Data);
			return 0;
		}
	}
	offset = RTPages + OTPages + (BTPages/2);
	for (int i = 0; i < BTPages / 2; ++i) {
		long* src = (long*)Uncompressed32Data + (i + offset) * ((256 * 256));
		TEXTURE* tex = &lvl->Textures[offset + i + 1];
		tex->width = (unsigned short)App.BumpMapSize;
		tex->height = (unsigned short)App.BumpMapSize;
		tex->bump = 0;
		if(!CreateTexturePage(tex->width, tex->height, fmt,b8g8r8a8, CalcMipMapCount(tex->width,tex->height), src, NULL, &tex->hal) ) {
			Log(-1, "Could not create texture page for texture %d",i);
			free(Uncompressed32Data);
			return 0;
		}
	}
	free(Uncompressed32Data);
	offset = RTPages + OTPages + BTPages;
	if(gfCurrentLevel == 0) {
		Log(1, "Loading uslogo.pak");
		char* pComp;
		char* logoCompressed = NULL;
		long size = LoadFile("data/uslogo.pak",&logoCompressed);
		long uncompSize = *(long*)logoCompressed; // logo files contain uncompSize, compSize, compData
		pComp = (char*)malloc(uncompSize);
		if(!S_Decompress(pComp, logoCompressed + 4, size - 4, uncompSize)) {
			Log(-1, "Error decompressing logo! Filesize: %d Decompression size: %d",size,uncompSize);
		}
		for(int i =0; i < 2; ++i) {
			for(int i =0; i < 2; ++i) {
				long* data = (long*)calloc(uncompSize,1);
				char* source = pComp + (i * 768);
				long* d = data;
				for(int y = 0; y < 256; y++) {
					for(int x = 0; x < 256; x++) {
						char r = *(source + (x * 3) + (y * 1536));
						char g = *(source + (x * 3) + (y * 1536) + 1);
						char b = *(source + (x * 3) + (y * 1536) + 2);
						char a = -1;

						if(!r && !b && !g)
							a = 0;

						long c = RGBA(r, g, b, a);
						*d++ = c;
					}
				}
				TEXTURE* logoTex = &lvl->Textures[offset+i+1];
				logoTex->width = 256;
				logoTex->height = 256;
				if(!CreateTexturePage(logoTex->width, logoTex->height,b8g8r8a8, b8g8r8a8, 0, data, NULL, &logoTex->hal)) {
					Log(-1, "Could not create texture page for Logo %d",i);
					free(Uncompressed32Data);
					free(data);
					return 0;
				}
			}
		}
		free(logoCompressed);
		free(pComp);
		offset += 2;
	}
	char mscMarker[3];
	fread(&mscMarker[0], 1, 3, f);
	if(!(mscMarker[0] == 'M' && mscMarker[1] == 'S' && mscMarker[2] == 'C')) {
		Log(-1, "Invalid Marker! %c %c %c", mscMarker[0],mscMarker[1],mscMarker[2]);
	}
	long miscUncompSize;
	long miscCompSize;
	fread(&miscUncompSize, 4, 1, f);
	fread(&miscCompSize,4,1,f);
	char* miscUncompressed32Data = (char*)calloc(miscUncompSize, 1);
	char* miscCompressed = (char*)calloc(miscCompSize, 1);
	fread(miscCompressed, miscCompSize, 1, f);
	if(!S_Decompress(miscUncompressed32Data, miscCompressed, miscCompSize, miscUncompSize)) {
		Log(-1, "Error decompressing font image!");
		free(miscCompressed);
		free(miscUncompressed32Data);
		return 0;
	}
	for(int i =0; i < 2; i++) {
		long* src = (long*)miscUncompressed32Data + (i) * ((256 * 256));
		TEXTURE* tex = &lvl->Textures[i+offset+1];
		tex->width = 256;
		tex->height = 256;
		if(!CreateTexturePage(tex->width, tex->height, fmt,b8g8r8a8, CalcMipMapCount(tex->width,tex->height), src, NULL, &tex->hal)) {
			Log(-1, "Error creating texture page for misc texture %d",i);
			free(miscCompressed);
			free(miscUncompressed32Data);
			return 0;
		}
	}
	return 1;
}

HAL_TEXTURE* GetRendererTexture(LEVEL_INFO* lvl, long num) {
	return &(lvl->Textures + num)->hal;
}

long GetNumTextures(LEVEL_INFO* lvl) {
	return lvl->nTextures;
}

HAL_TEXTURE* GetRendererBumpTexture(LEVEL_INFO* lvl, long num) {
	return GetRendererTexture(lvl, (lvl->Textures + num)->bumptpage);
}
char HasRendererBumpTexture(LEVEL_INFO* lvl, long num) {
	if(num < 0) {
		return 0;
	}
	return (lvl->Textures + num)->bump;
}

TEXTURESTRUCT* GetTextInfo(LEVEL_INFO* lvl, long num) {
	return lvl->textinfo + num;
}