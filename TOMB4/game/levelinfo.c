#include "game/levelinfo.h"
#include "game/animstruct.h"
#include "game/changestruct.h"
#include "game/control.h"
#include "specific/d3dmatrix.h"
#include "specific/drawroom.h"
#include "specific/dxshell.h"
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
#include "specific/polyinsert.h"
#include "game/rangestruct.h"
#include "game/roominfo.h"
#include "game/samplebuffer.h"
#include "game/sampleinfo.h"
#include "game/setup.h"
#include "game/staticinfo.h"
#include <ddraw.h>
#include <stdlib.h>
#include "game/floorinfo.h"
#include "game/lightinfo.h"
#include "game/meshinfo.h"
#include "global/types.h"
#include "specific/winmain.h"
#include "game/boxinfo.h"
#include "specific/dxsound.h"

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
};

LEVEL_INFO* CreateLevel() {
	LEVEL_INFO* lvl = (LEVEL_INFO*)calloc(1,sizeof(LEVEL_INFO));
	lvl->objects = (OBJECT_INFO*)calloc(NUMBER_OBJECTS,sizeof(OBJECT_INFO));
	lvl->statics = (STATIC_INFO*)calloc(NUMBER_STATIC_OBJECTS, sizeof(STATIC_INFO));
	lvl->effects = (FX_INFO*)calloc(128, sizeof(FX_INFO));
	lvl->effectsCapacity = 128;
	lvl->items = (ITEM_INFO*)calloc(1024, sizeof(ITEM_INFO));
	lvl->itemsCapacity = 1024;
	return lvl;
}

bool LoadObjects(char** FileData,LEVEL_INFO* lvl) {
	OBJECT_INFO* obj;
	STATIC_INFO* stat;
	short** mesh;
	short** mesh_size;
	long size, num, slot;
	static long num_meshes, num_anims;

	size = *(long*)*FileData;
	*FileData += sizeof(long);
	lvl->mesh_base = (short*)calloc(size, sizeof(short));
	memcpy(lvl->mesh_base, *FileData, size * sizeof(short));
	*FileData += size * sizeof(short);

	size = *(long*)*FileData;
	*FileData += sizeof(long);
	lvl->meshes = (short**)calloc(2 * size, sizeof(short*));
	memcpy(lvl->meshes, *FileData, size * sizeof(short*));
	*FileData += size * sizeof(short*);

	for(int i = 0; i < size; i++)
		lvl->meshes[i] = lvl->mesh_base + (long)(uintptr_t)(lvl->meshes[i]) / 2;

	num_meshes = size;

	num_anims = *(long*)*FileData;
	*FileData += sizeof(long);
	lvl->anims = (ANIM_STRUCT*)calloc(num_anims,sizeof(ANIM_STRUCT));
	memcpy(lvl->anims, *FileData, sizeof(ANIM_STRUCT) * num_anims);
	*FileData += sizeof(ANIM_STRUCT) * num_anims;

	size = *(long*)*FileData;
	*FileData += sizeof(long);
	lvl->changes = (CHANGE_STRUCT*)calloc(size,sizeof(CHANGE_STRUCT));
	memcpy(lvl->changes, *FileData, sizeof(CHANGE_STRUCT) * size);
	*FileData += sizeof(CHANGE_STRUCT) * size;

	size = *(long*)*FileData;
	*FileData += sizeof(long);
	lvl->ranges = (RANGE_STRUCT*)calloc(size,sizeof(RANGE_STRUCT) );
	memcpy(lvl->ranges, *FileData, sizeof(RANGE_STRUCT) * size);
	*FileData += sizeof(RANGE_STRUCT) * size;

	size = *(long*)*FileData;
	*FileData += sizeof(long);
	lvl->commands = (short*)calloc(size,sizeof(short));
	memcpy(lvl->commands, *FileData, sizeof(short) * size);
	*FileData += sizeof(short) * size;

	size = *(long*)*FileData;
	*FileData += sizeof(long);
	lvl->bones = (long*)calloc(size,sizeof(long));
	memcpy(lvl->bones, *FileData, sizeof(long) * size);
	*FileData += sizeof(long) * size;

	size = *(long*)*FileData;
	*FileData += sizeof(long);
	lvl->frames = (short*)calloc(size,sizeof(short));
	memcpy(lvl->frames, *FileData, sizeof(short) * size);
	*FileData += sizeof(short) * size;

	for(int i = 0; i < num_anims; i++)
		lvl->anims[i].frame_ptr = (short*)((long)lvl->anims[i].frame_ptr + (long)lvl->frames);

	num = *(long*)*FileData;
	*FileData += sizeof(long);

	for(int i = 0; i < num; i++) {
		slot = *(long*)*FileData;
		*FileData += sizeof(long);
		obj = GetObjectInfo(currentLevel,slot);

		obj->nmeshes = *(short*)*FileData;
		*FileData += sizeof(short);

		obj->mesh_index = *(short*)*FileData;
		*FileData += sizeof(short);

		obj->bone_index = *(long*)*FileData;
		*FileData += sizeof(long);

		obj->frame_base = (short*)(*(short**)*FileData);
		*FileData += sizeof(short*);

		obj->anim_index = *(short*)*FileData;
		*FileData += sizeof(short);

		obj->loaded = 1;
	}

	CreateSkinningData();

	for(int i = 0; i < NUMBER_OBJECTS; i++) {
		obj = GetObjectInfo(currentLevel,i);
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

	num = *(long*)*FileData; // statics
	*FileData += sizeof(long);

	for(int i = 0; i < num; i++) {
		slot = *(long*)*FileData;
		*FileData += sizeof(long);
		stat = GetStaticObject(currentLevel,slot);

		stat->mesh_number = *(short*)*FileData;
		*FileData += sizeof(short);

		memcpy(&stat->x_minp, *FileData, 6 * sizeof(short));
		*FileData += 6 * sizeof(short);

		memcpy(&stat->x_minc, *FileData, 6 * sizeof(short));
		*FileData += 6 * sizeof(short);

		stat->flags = *(short*)*FileData;
		*FileData += sizeof(short);
	}

	for(int i = 0; i < NUMBER_STATIC_OBJECTS; i++) {
		stat = GetStaticObject(currentLevel,i);
		stat->mesh_number *= 2;
	}

	ProcessMeshData(lvl,num_meshes * 2);
	return 1;
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

short** GetMeshPointer(LEVEL_INFO* lvl, long mesh) {
	return lvl->meshes+mesh;
}

short* GetMesh(LEVEL_INFO* lvl, long mesh) {
	return lvl->meshes[mesh];
}
short* GetMeshBase(LEVEL_INFO* lvl) {
	return lvl->mesh_base;
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

void ProcessMeshData(LEVEL_INFO* lvl, long num_meshes) {
	MESH_DATA* mesh;
	D3DVERTEX* vtx;
	D3DVERTEXBUFFERDESC buf;
	short* mesh_ptr;
	short* last_mesh_ptr;
	long lp;
	short c;

	Log(2, "ProcessMeshData %d", num_meshes);
	num_level_meshes = num_meshes;
	mesh_vtxbuf = (MESH_DATA**)malloc(4 * num_meshes);
	lvl->mesh_base = (short*)mesh_vtxbuf;
	last_mesh_ptr = 0;
	mesh = (MESH_DATA*)(uintptr_t)num_meshes;

	for(int i = 0; i < num_meshes; i++) {
		mesh_ptr = GetMesh(currentLevel,i);

		if(mesh_ptr == last_mesh_ptr) {
			lvl->meshes[i] = (short*)mesh;
			mesh_vtxbuf[i] = mesh;
		} else {
			last_mesh_ptr = mesh_ptr;
			mesh = (MESH_DATA*)malloc(sizeof(MESH_DATA));
			memset(mesh, 0, sizeof(MESH_DATA));
			*GetMeshPointer(currentLevel,i) = (short*)mesh;
			mesh_vtxbuf[i] = mesh;
			mesh->x = mesh_ptr[0];
			mesh->y = mesh_ptr[1];
			mesh->z = mesh_ptr[2];
			mesh->r = mesh_ptr[3];
			mesh->flags = mesh_ptr[4];
			mesh->nVerts = mesh_ptr[5] & 0xFF;
			lp = 0;

			if(!mesh->nVerts)
				lp = mesh_ptr[5] >> 8;

			mesh_ptr += 6;

			if(mesh->nVerts) {
				buf.dwNumVertices = mesh->nVerts;
				buf.dwSize = sizeof(D3DVERTEXBUFFERDESC);
				buf.dwCaps = 0;
				buf.dwFVF = D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ;
				DXAttempt(IDirect3D3_CreateVertexBuffer(App.dx.lpD3D,&buf, &mesh->SourceVB, 0, 0));
				DXAttempt(IDirect3DVertexBuffer7_Lock(mesh->SourceVB, DDLOCK_WRITEONLY, (LPVOID*)&vtx, 0));
				IDirect3DVertexBuffer_Lock(mesh->SourceVB, DDLOCK_WRITEONLY, (LPVOID*)&vtx, 0);

				for(int j = 0; j < mesh->nVerts; j++) {
					vtx[j].x = mesh_ptr[0];
					vtx[j].y = mesh_ptr[1];
					vtx[j].z = mesh_ptr[2];
					mesh_ptr += 3;
				}

				mesh->nNorms = mesh_ptr[0];
				mesh_ptr++;

				if(!mesh->nNorms)
					mesh->nNorms = mesh->nVerts;

				if(mesh->nNorms > 0) {
					mesh->Normals = (_D3DVECTOR*)malloc(mesh->nNorms * sizeof(_D3DVECTOR));

					for(int j = 0; j < mesh->nVerts; j++) {
						vtx[j].nx = mesh_ptr[0];
						vtx[j].ny = mesh_ptr[1];
						vtx[j].nz = mesh_ptr[2];
						mesh_ptr += 3;
						D3DNormalise((_D3DVECTOR*)&vtx[j].nx);
						mesh->Normals[j].x = vtx[j].nx;
						mesh->Normals[j].y = vtx[j].ny;
						mesh->Normals[j].z = vtx[j].nz;
					}

					mesh->prelight = 0;
				} else {
					mesh->Normals = 0;
					mesh->prelight = (long*)malloc(4 * mesh->nVerts);

					for(int j = 0; j < mesh->nVerts; j++) {
						c = 255 - (mesh_ptr[0] >> 5);
						mesh->prelight[j] = RGBONLY(c, c, c);
						mesh_ptr++;
					}
				}

				IDirect3DVertexBuffer_Unlock(mesh->SourceVB);
			} else
				mesh_ptr += 6 * lp + 1;

			mesh->ngt4 = mesh_ptr[0];
			mesh_ptr++;

			if(mesh->ngt4) {
				mesh->gt4 = (short*)malloc(12 * mesh->ngt4);
				lp = 6 * mesh->ngt4;

				for(int j = 0; j < lp; j++)
					mesh->gt4[j] = mesh_ptr[j];

				mesh_ptr += lp;
			}

			mesh->ngt3 = mesh_ptr[0];
			mesh_ptr++;

			if(mesh->ngt3) {
				mesh->gt3 = (short*)malloc(10 * mesh->ngt3);
				lp = 5 * mesh->ngt3;

				for(int j = 0; j < lp; j++)
					mesh->gt3[j] = mesh_ptr[j];
			}
		}
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
RANGE_STRUCT* GetAnimRange(LEVEL_INFO* lvl,long index) {
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

bool LoadItems(char** FileData, LEVEL_INFO* lvl) {
	ITEM_INFO* item;
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	STATIC_INFO* stat;
	
	long num_items, x, y, z;

	Log(2, "LoadItems");
	num_items = *(long*)*FileData;
	*FileData += 4;

	if(!num_items)
		return 1;

	lvl->level_items = num_items;
	InitialiseItemArray((short)lvl->itemsCapacity,lvl->level_items);

	for(int i = 0; i < num_items; i++) {
		item = GetItem(lvl,i);

		item->object_number = *(short*)*FileData;
		*FileData += sizeof(short);

		item->room_number = *(short*)*FileData;
		*FileData += sizeof(short);

		item->pos.x_pos = *(long*)*FileData;
		*FileData += sizeof(long);

		item->pos.y_pos = *(long*)*FileData;
		*FileData += sizeof(long);

		item->pos.z_pos = *(long*)*FileData;
		*FileData += sizeof(long);

		item->pos.y_rot = *(short*)*FileData;
		*FileData += sizeof(short);

		item->shade = *(short*)*FileData;
		*FileData += sizeof(short);

		item->trigger_flags = *(short*)*FileData;
		*FileData += sizeof(short);

		item->flags = *(short*)*FileData;
		*FileData += sizeof(short);
	}

	for(int i = 0; i < num_items; i++)
		InitialiseItem(i);

	for(int i = 0; i < GetNumRooms(currentLevel); i++) {
		r = GetRoom(currentLevel,i);

		for(int j = 0; j < r->num_meshes; j++) {
			x = (r->mesh[j].x - r->x) >> 10;
			z = (r->mesh[j].z - r->z) >> 10;

			floor = &(r->floor[x * r->x_size + z]);

			if(!(lvl->boxes[floor->box].overlap_index & 0x4000)) {
				stat = GetStaticObject(currentLevel,r->mesh[j].static_number);
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

short GetItemNum(LEVEL_INFO *lvl, ITEM_INFO *i) {
	return i - lvl->items;
}

long GetNumBoxes(LEVEL_INFO *lvl) {
	return lvl->num_boxes;
}

bool LoadBoxes(char** FileData,LEVEL_INFO* lvl) {
	BOX_INFO* box;
	long size;

	Log(2, "LoadBoxes");
	lvl->num_boxes = *(long*)*FileData;
	*FileData += sizeof(long);

	lvl->boxes = (BOX_INFO*)calloc(lvl->num_boxes,sizeof(BOX_INFO));
	memcpy(lvl->boxes, *FileData, sizeof(BOX_INFO) * lvl->num_boxes);
	*FileData += sizeof(BOX_INFO) * lvl->num_boxes;

	size = *(long*)*FileData;
	*FileData += sizeof(long);
	lvl->overlaps = (unsigned short*)calloc(size,sizeof(unsigned short));
	memcpy(lvl->overlaps, *FileData, sizeof(unsigned short) * size);
	*FileData += sizeof(unsigned short) * size;

	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 4; j++) {
			lvl->ground_zone[j][i] = (short*)calloc(lvl->num_boxes,sizeof(short));
			memcpy(lvl->ground_zone[j][i], *FileData, sizeof(short) * lvl->num_boxes);
			*FileData += sizeof(short) * lvl->num_boxes;
		}

		lvl->ground_zone[4][i] = (short*)calloc(lvl->num_boxes,sizeof(short));
		memcpy(lvl->ground_zone[4][i], *FileData, sizeof(short) * lvl->num_boxes);
		*FileData += sizeof(short) * lvl->num_boxes;
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

long GetNumLevelItems(LEVEL_INFO *lvl) {
	return lvl->level_items;
}

BOX_INFO* GetBox(LEVEL_INFO* lvl, long box) {
	return lvl->boxes + box;
}

unsigned short* GetOverlap(LEVEL_INFO* lvl, long overlap) {
	return lvl->overlaps + overlap;
}

bool LoadSamples(FILE* level_fp, char** FileData,LEVEL_INFO* lvl) {
	long num_samples, uncomp_size, comp_size;
	static long num_sample_infos;

	Log(2, "LoadSamples");
	lvl->sample_lut = (short*)calloc(MAX_SAMPLES, sizeof(short));
	memcpy(lvl->sample_lut, *FileData, MAX_SAMPLES * sizeof(short));
	*FileData += MAX_SAMPLES * sizeof(short);
	num_sample_infos = *(long*)*FileData;
	*FileData += sizeof(long);
	Log(8, "Number Of Sample Infos %d", num_sample_infos);

	if(!num_sample_infos) {
		Log(1, "No Sample Infos");
		return 0;
	}

	lvl->sample_infos = (SAMPLE_INFO*)calloc(lvl->num_sample_infos,sizeof(SAMPLE_INFO));
	memcpy(lvl->sample_infos, *FileData, sizeof(SAMPLE_INFO) * num_sample_infos);
	*FileData += sizeof(SAMPLE_INFO) * num_sample_infos;
	num_samples = *(long*)*FileData;
	*FileData += sizeof(long);

	if(!num_samples) {
		Log(1, "No Samples");
		return 0;
	}

	Log(8, "Number Of Samples %d", num_samples);
	lvl->sample_buffers = (SAMPLE_BUFFER*)calloc(num_samples,	sizeof(SAMPLE_BUFFER));
	
	fread(&num_samples, 1, 4, level_fp);

	if(num_samples <= 0) {
		return 1;
	}

	for(int i = 0; i < num_samples; i++) {
		unsigned char* decompression_buffer = (unsigned char*)calloc(0xF1000,sizeof(unsigned char));
		fread(&uncomp_size, 1, 4, level_fp);
		fread(&comp_size, 1, 4, level_fp);
		fread(decompression_buffer, comp_size, 1, level_fp);

		if(!S_ConvertSamples(decompression_buffer, comp_size, uncomp_size, i,lvl->sample_buffers)) {
			free(decompression_buffer);
			return 0;
		}
		free(decompression_buffer);
	}
	return 1;
}

SAMPLE_BUFFER* GetSampleBuffer(LEVEL_INFO* lvl, long num) {
	return lvl->sample_buffers+num;
}

SAMPLE_INFO* GetSampleInfo(LEVEL_INFO* lvl, long num) {
	return lvl->sample_infos + num;
}

short* GetSampleLookup(LEVEL_INFO* lvl, long num) {
	return lvl->sample_lut + num;
}