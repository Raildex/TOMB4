
#include "specific/function_stubs.h"
#include <windows.h>
#include <stdbool.h>
static long long counter, frequency;

long Sync() {
	long long PerformanceCount, f;
	long n;

	QueryPerformanceCounter((LARGE_INTEGER*)&PerformanceCount);
	f = (PerformanceCount - counter) / frequency;
	counter += frequency * f;
	n = (long)f;
	return n;
}

void TIME_Reset() {
	QueryPerformanceCounter((LARGE_INTEGER*)&counter);
}

bool TIME_Init() {
	long long pfq;

	Log(__func__, "TIME_Init");

	if(!QueryPerformanceFrequency((LARGE_INTEGER*)&pfq))
		return 0;

	frequency = pfq / 60;
	TIME_Reset();
	return 1;
}
