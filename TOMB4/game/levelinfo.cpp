#include "levelinfo.h"
#include "objectinfo.h"
#include "objects.h"
#include "staticinfo.h"
#include <cstdlib>

LEVEL_INFO* currentLevel;
struct LEVEL_INFO {
	OBJECT_INFO* objects;
	STATIC_INFO* statics;
};

LEVEL_INFO* CreateLevel() {
	LEVEL_INFO* lvl = (LEVEL_INFO*)malloc(sizeof(LEVEL_INFO));
	lvl->objects = (OBJECT_INFO*)calloc(NUMBER_OBJECTS,sizeof(OBJECT_INFO));
	lvl->statics = (STATIC_INFO*)calloc(NUMBER_STATIC_OBJECTS, sizeof(STATIC_INFO));
	return lvl;
}

void DestroyLevel(LEVEL_INFO* lvl) {
	free(lvl->objects);
	free(lvl->statics);
	free(lvl);
}

void LoadLevel(LEVEL_INFO *lvl, char *file) {

}

OBJECT_INFO* GetObjectInfo(LEVEL_INFO* lvl, long type) {
	return lvl->objects + type;
}

STATIC_INFO* GetStaticObject(LEVEL_INFO* lvl, long type) {
	return lvl->statics + type;
}