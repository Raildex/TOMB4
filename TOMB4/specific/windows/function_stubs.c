
#include <stdio.h>
#include <stdarg.h>
#include "specific/function_stubs.h"
#include "game/phdvector.h"

FILE* logF = 0;

PHD_VECTOR CamPos;
PHD_VECTOR CamRot;

long nPolyType;

static long rand_1 = 0xD371F947;
static long rand_2 = 0xD371F947;

long GetRandomControl() {
	rand_1 = 0x41C64E6D * rand_1 + 12345;
	return (rand_1 >> 10) & 0x7FFF;
}

void SeedRandomControl(long seed) {
	rand_1 = seed;
}

long GetRandomDraw() {
	rand_2 = 0x41C64E6D * rand_2 + 12345;
	return (rand_2 >> 10) & 0x7FFF;
}

void SeedRandomDraw(long seed) {
	rand_2 = seed;
}

void Log(const char* scope, const char* s, ...) {
	fprintf(stderr, "%s : ", scope);
	va_list list;
	va_start(list, s);
	vfprintf(stdout, s, list);
	fprintf(stdout, "\n");
	va_end(list);
}

void LogE(const char* scope, const char* s, ...) {
	fprintf(stderr, "%s : ", scope);
	va_list list;
	va_start(list, s);
	vfprintf(stderr, s, list);
	fprintf(stderr, "\n");
	va_end(list);
	fflush(stderr);
}

void LogD(const char* scope, const char* s, ...) {
	#if defined(NDEBUG) && defined (DO_LOG)
	fprintf(stdout, "%s : ", scope);
	va_list list;

	va_start(list, s);
	vfprintf(stderr, s, list);
	fprintf(stderr, "\n");
	va_end(list);
	fflush(stderr);
	#endif
}
