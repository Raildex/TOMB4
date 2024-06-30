#include <zlib.h>
#include "specific/file.h"
#include "specific/function_stubs.h"
#include "specific/texture.h"
#include "specific/drawbars.h"
#include "specific/drawroom.h"
#include "game/setup.h"
#include "game/objects.h"
#include "specific/specificfx.h"
#include "game/tomb4fx.h"
#include "specific/audio.h"
#include "specific/polyinsert.h"
#include "specific/winmain.h"
#include "game/lara.h"
#include "specific/output.h"
#include "game/gameflow.h"
#include "specific/texture.h"
#include "game/objectinfo.h"
#include "game/meshdata.h"
#include "game/texturestruct.h"
#include <d3dtypes.h>
#include "specific/texture.h"
#include "game/larainfo.h"
#include <process.h>
#include "game/levelinfo.h"
#include "texture.h"

THREAD LevelLoadingThread;

TEXTURESTRUCT* AnimatingWaterfalls[3];
long AnimatingWaterfallsV[3];

long number_cameras;

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
		S_GetUVRotateTextures(currentLevel);

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


char LoadCinematic(char** data, LEVEL_INFO* lvl) {
	*data += sizeof(short);
	return 1;
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
