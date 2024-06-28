#include <zlib.h>
#include "specific/file.h"
#include "specific/function_stubs.h"
#include "specific/texture.h"
#include "specific/dxsound.h"
#include "specific/drawbars.h"
#include "specific/dxshell.h"
#include "specific/drawroom.h"
#include "game/setup.h"
#include "game/objects.h"
#include "specific/specificfx.h"
#include "game/tomb4fx.h"
#include "specific/audio.h"
#include "game/spotcam.h"
#include "game/control.h"
#include "game/camera.h"
#include "specific/polyinsert.h"
#include "game/draw.h"
#include "game/effects.h"
#include "specific/winmain.h"
#include "game/lara.h"
#include "specific/output.h"
#include "game/gameflow.h"
#include "specific/dxinfo.h"
#include "specific/dxdirectdrawinfo.h"
#include "specific/dxd3ddevice.h"
#include "specific/dxtextureinfo.h"
#include "specific/texture.h"
#include "game/objectinfo.h"
#include "game/meshdata.h"
#include "game/texturestruct.h"
#include <d3dtypes.h>
#include "game/phdspritestruct.h"
#include "game/spritestruct.h"
#include "game/objectvector.h"
#include "game/phdtexturestruct.h"
#include "game/staticinfo.h"
#include "game/aiobject.h"
#include "specific/texture.h"
#include "game/larainfo.h"
#include "game/languages.h"
#include <process.h>
#include "game/levelinfo.h"
#include "texture.h"


SPRITESTRUCT* spriteinfo;
THREAD LevelLoadingThread;

TEXTURESTRUCT* AnimatingWaterfalls[3];
long AnimatingWaterfallsV[3];

AIOBJECT* AIObjects;
short* aranges;
long nAnimUVRanges;
long number_cameras;
short nAIObjects;

static char* CompressedData;

unsigned int WINAPI LoadLevel(void* name) {
	OBJECT_INFO* obj;
	TEXTURESTRUCT* tex;
	char* pData;
	long version, size, compressedSize;

	Log(2, "LoadLevel");
	FreeLevel();
	currentLevel = CreateLevel();

	memset(malloc_ptr, 0, MALLOC_SIZE);
	memset(&lara, 0, sizeof(LARA_INFO));

	S_InitLoadBar(20);
	//S_LoadBar();

	CompressedData = NULL;
	char* FileData = NULL;
	FILE* level_fp = NULL;
	level_fp = FileOpen((const char*)name);

	if(level_fp) {
		fread(&version, 4, 1, level_fp);
		Log(7, "Process Level Data");
		LoadTextures(b8g8r8a8,level_fp,currentLevel);
		char geoMarker[3];
		fread(&geoMarker[0], 1, 3, level_fp);
		if(!(geoMarker[0] == 'G' && geoMarker[1] == 'E' && geoMarker[2] == 'O')) {
			Log(-1, "Invalid Marker %c %c %c!",geoMarker[0],geoMarker[1],geoMarker[2]);
		}
		fread(&size, 1, 4, level_fp);
		fread(&compressedSize, 1, 4, level_fp);
		CompressedData = (char*)calloc(1 ,compressedSize);
		FileData = (char*)calloc(1,size);
		fread(CompressedData, compressedSize, 1u, level_fp);
		S_Decompress(FileData, CompressedData, compressedSize, size);
		free(CompressedData);

		pData = FileData;
		S_LoadBar();

		LoadRooms(&FileData,currentLevel);
		S_LoadBar();

		LoadObjects(&FileData,currentLevel);
		S_LoadBar();

		LoadSprites(&FileData, currentLevel);
		S_LoadBar();

		LoadCameras(&FileData, currentLevel);
		S_LoadBar();

		LoadSoundEffects(&FileData, currentLevel);
		S_LoadBar();

		LoadBoxes(&FileData,currentLevel);
		S_LoadBar();

		LoadAnimatedTextures(&FileData, currentLevel);
		S_LoadBar();

		LoadTextureInfos(&FileData, currentLevel);
		S_LoadBar();

		LoadItems(&FileData,currentLevel);
		S_LoadBar();

		LoadAIInfo(&FileData, currentLevel);
		S_LoadBar();

		LoadCinematic(&FileData, currentLevel);
		S_LoadBar();
		char sfxMarker[3];
		fread(&sfxMarker[0], 1, 3, level_fp);
		if(!(sfxMarker[0] == 'S' && sfxMarker[1] == 'F' && sfxMarker[2] == 'X')) {
			Log(-1, "Invalid Marker %c %c %c!",sfxMarker[0],sfxMarker[1],sfxMarker[2]);
		}
		if(acm_ready && !App.SoundDisabled)
			LoadSamples(level_fp, &FileData, currentLevel);

		free(pData);
		S_LoadBar();

		for(int i = 0; i < 3; i++) {
			obj = GetObjectInfo(currentLevel,WATERFALL1 + i);

			if(obj->loaded) {
				tex = GetTextInfo(currentLevel,mesh_vtxbuf[obj->mesh_index]->gt4[4] & 0x7FFF);
				AnimatingWaterfalls[i] = tex;
				AnimatingWaterfallsV[i] = (long)tex->v1;
			}
		}

		S_LoadBar();
		S_GetUVRotateTextures();

		InitTarget_2();
		S_LoadBar();

		SetupGame();
		S_LoadBar();

		SetFadeClip(0, 1);
		reset_cutseq_vars();
		FileClose(level_fp);
	}else {
		Log(-1, "Could not open Level file!");
		return 0;
	}

	LevelLoadingThread.active = 0;
	_endthreadex(1);
	return 1;
}

long S_LoadLevelFile(long num) {
	char name[80];

	Log(2, "S_LoadLevelFile");
	strcpy(name, &gfFilenameWad[gfFilenameOffset[num]]);
	strcat(name, ".TR4");
	LevelLoadingThread.active = 1;
	LevelLoadingThread.ended = 0;
	LevelLoadingThread.handle = _beginthreadex(0, 0, &LoadLevel, name, 0, (unsigned int*)&LevelLoadingThread.address);
	while(LevelLoadingThread.active)
		;
	return 1;
}

void FreeLevel() {
	if(currentLevel)
		DestroyLevel(currentLevel);
	malloc_ptr = malloc_buffer;
	malloc_free = malloc_size;
}

bool FindCDDrive() {
	HANDLE file;
	unsigned long drives, type;
	char path[14];
	char root[5];
	static char cd_drive;

	strcpy(path, "c:\\script.dat");
	drives = GetLogicalDrives();
	cd_drive = 'A';
	lstrcpy(root, "A:\\");

	while(drives) {
		if(drives & 1) {
			root[0] = cd_drive;
			type = GetDriveType(root);

			if(type == DRIVE_CDROM) {
				path[0] = cd_drive;
				file = CreateFile(path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

				if(file != INVALID_HANDLE_VALUE) {
					CloseHandle(file);
					return 1;
				}
			}
		}

		cd_drive++;
		drives >>= 1;
	}

	return 0;
}

FILE* FileOpen(const char* name) {
	FILE* file;
	char path_name[80];

	memset(path_name, 0, 80);
	strcat(path_name, name);
	Log(5, "FileOpen - %s", path_name);
	file = fopen(path_name, "rb");

	if(!file)
		Log(1, "Unable To Open %s", path_name);

	return file;
}

void FileClose(FILE* file) {
	Log(2, "FileClose");
	fclose(file);
}

long FileSize(FILE* file) {
	long size;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	return size;
}

long LoadFile(const char* name, char** dest) {
	FILE* file;
	long size, count;

	Log(2, "LoadFile");
	Log(5, "File - %s", name);
	file = FileOpen(name);

	if(!file)
		return 0;

	size = FileSize(file);

	if(!*dest)
		*dest = (char*)malloc(size);

	count = fread(*dest, 1, size, file);
	Log(5, "Read - %d FileSize - %d", count, size);

	if(count != size) {
		Log(1, "Error Reading File");
		FileClose(file);
		free(*dest);
		return 0;
	}

	FileClose(file);
	return size;
}



char LoadSprites(char** data, LEVEL_INFO* lvl) {
	STATIC_INFO* stat;
	OBJECT_INFO* obj;
	SPRITESTRUCT* sptr;
	PHDSPRITESTRUCT sprite;
	long num_sprites, num_slots, slot;

	Log(2, "LoadSprites");
	*data += 3;
	num_sprites = *(long*)*data;
	*data += sizeof(long);
	spriteinfo = (SPRITESTRUCT*)game_malloc(sizeof(SPRITESTRUCT) * num_sprites);

	for(int i = 0; i < num_sprites; i++) {
		sptr = &spriteinfo[i];
		memcpy(&sprite, *data, sizeof(PHDSPRITESTRUCT));
		*data += sizeof(PHDSPRITESTRUCT);
		sptr->height = sprite.height;
		sptr->offset = sprite.offset;
		sptr->tpage = sprite.tpage;
		sptr->width = sprite.width;
		sptr->x1 = (float)((sprite.x1) * (1.0F / 256.0F));
		sptr->y1 = (float)((sprite.y1) * (1.0F / 256.0F));
		sptr->x2 = (float)((sprite.x2) * (1.0F / 256.0F));
		sptr->y2 = (float)((sprite.y2) * (1.0F / 256.0F));
		sptr->x1 += (1.0F / 256.0F);
		sptr->y1 += (1.0F / 256.0F);
		sptr->x2 -= (1.0F / 256.0F);
		sptr->y2 -= (1.0F / 256.0F);
		sptr->tpage++;
	}

	num_slots = *(long*)*data;
	*data += sizeof(long);

	if(num_slots <= 0)
		return 1;

	for(int i = 0; i < num_slots; i++) {
		slot = *(long*)*data;
		*data += sizeof(long);

		if(slot >= NUMBER_OBJECTS) {
			slot -= NUMBER_OBJECTS;
			stat = GetStaticObject(currentLevel,slot);
			stat->mesh_number = *(short*)*data;
			*data += sizeof(short);
			stat->mesh_number = *(short*)*data;
			*data += sizeof(short);
		} else {
			obj = GetObjectInfo(currentLevel,slot);
			obj->nmeshes = *(short*)*data;
			*data += sizeof(short);
			obj->mesh_index = *(short*)*data;
			*data += sizeof(short);
			obj->loaded = 1;
		}
	}

	return 1;
}

char LoadCameras(char** data, LEVEL_INFO* lvl) {
	Log(2, "LoadCameras");
	number_cameras = *(long*)*data;
	*data += sizeof(long);

	if(number_cameras) {
		camera.fixed = (OBJECT_VECTOR*)game_malloc(number_cameras * sizeof(OBJECT_VECTOR));
		memcpy(camera.fixed, *data, number_cameras * sizeof(OBJECT_VECTOR));
		*data += number_cameras * sizeof(OBJECT_VECTOR);
	}

	number_spotcams = *(short*)*data;
	*data += sizeof(long); //<<---- look at me

	if(number_spotcams) {
		memcpy(SpotCam, *data, number_spotcams * sizeof(SPOTCAM));
		*data += number_spotcams * sizeof(SPOTCAM);
	}

	return 1;
}

char LoadSoundEffects(char** data, LEVEL_INFO* lvl) {
	Log(2, "LoadSoundEffects");
	number_sound_effects = *(long*)*data;
	*data += sizeof(long);
	Log(8, "Number of SFX %d", number_sound_effects);

	if(number_sound_effects) {
		sound_effects = (OBJECT_VECTOR*)game_malloc(number_sound_effects * sizeof(OBJECT_VECTOR));
		memcpy(sound_effects, *data, number_sound_effects * sizeof(OBJECT_VECTOR));
		*data += number_sound_effects * sizeof(OBJECT_VECTOR);
	}

	return 1;
}

char LoadAnimatedTextures(char** data, LEVEL_INFO* lvl) {
	long num_anim_ranges;

	num_anim_ranges = *(long*)*data;
	*data += sizeof(long);
	aranges = (short*)game_malloc(num_anim_ranges * 2);
	memcpy(aranges, *data, num_anim_ranges * 2);
	*data += num_anim_ranges * sizeof(short);
	nAnimUVRanges = *(char*)*data;
	*data += sizeof(char);
	return 1;
}


char LoadCinematic(char** data, LEVEL_INFO* lvl) {
	*data += sizeof(short);
	return 1;
}

char LoadAIInfo(char** data, LEVEL_INFO* lvl) {
	long num_ai;

	num_ai = *(long*)*data;
	*data += sizeof(long);

	if(num_ai) {
		nAIObjects = (short)num_ai;
		AIObjects = (AIOBJECT*)game_malloc(sizeof(AIOBJECT) * num_ai);
		memcpy(AIObjects, *data, sizeof(AIOBJECT) * num_ai);
		*data += sizeof(AIOBJECT) * num_ai;
	}

	return 1;
}



void S_GetUVRotateTextures() {
	TEXTURESTRUCT* tex;
	short* pRange;

	pRange = aranges + 1;

	for(int i = 0; i < nAnimUVRanges; i++, pRange++) {
		for(int j = (int)*(pRange++); j >= 0; j--, pRange++) {
			tex = GetTextInfo(currentLevel,*pRange);
			AnimatingTexturesV[i][j][0] = tex->v1;
		}

		pRange--;
	}
}



char S_Decompress(char* pDest, char* pCompressed, long compressedSize, long size) {
	z_stream stream;

	Log(2, "Decompress");
	memset(&stream, 0, sizeof(z_stream));
	stream.avail_in = compressedSize;
	stream.avail_out = size;
	stream.next_out = (Bytef*)pDest;
	stream.next_in = (Bytef*)pCompressed;
	inflateInit(&stream);
	inflate(&stream, Z_FINISH);

	if(stream.total_out != size) {
		Log(1, "Error Decompressing Data");
		return 0;
	}

	inflateEnd(&stream);
	Log(5, "Decompression OK");
	return 1;
}
