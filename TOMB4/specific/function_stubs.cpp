
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include "function_stubs.h"
#include "phdvector.h"
#include "types.h"

FILE* logF = 0;

PHD_VECTOR CamPos;
PHD_VECTOR CamRot;

long nPolyType;

char* malloc_buffer;
char* malloc_ptr;
long malloc_size;
long malloc_free;

static long malloc_used;

static long rand_1 = 0xD371F947;
static long rand_2 = 0xD371F947;

long GetRandomControl()
{
	rand_1 = 0x41C64E6D * rand_1 + 12345;
	return (rand_1 >> 10) & 0x7FFF;
}

void SeedRandomControl(long seed)
{
	rand_1 = seed;
}

long GetRandomDraw()
{
	rand_2 = 0x41C64E6D * rand_2 + 12345;
	return (rand_2 >> 10) & 0x7FFF;
}

void SeedRandomDraw(long seed)
{
	rand_2 = seed;
}

void init_game_malloc()
{
	malloc_buffer = (char*)malloc(MALLOC_SIZE);
	malloc_size = MALLOC_SIZE;
	malloc_ptr = malloc_buffer;
	malloc_free = MALLOC_SIZE;
	malloc_used = 0;
}

void* game_malloc(long size)
{
	char* ptr;

	size = (size + 3) & -4;

	if (size > malloc_free)
	{
		Log(0, "OUT OF MEMORY");
		return 0;
	}
	else
	{
		ptr = malloc_ptr;
		malloc_free -= size;
		malloc_used += size;
		malloc_ptr += size;
		memset(ptr, 0, size);
		return ptr;
	}
}

void Log(unsigned long type, const char* s, ...)
{
#ifdef DO_LOG
	va_list list;

	va_start(list, s);
	vfprintf(stdout,s,list);
	fprintf(stdout,"\n");
	va_end(list);
	fflush(stdout);
#endif
}
