
#include "specific/function_stubs.h"
#include <windows.h>
static int counter, frequency;

int S_Sync() {
	int PerformanceCount, f;
	int n;

	QueryPerformanceCounter((LARGE_INTEGER*)&PerformanceCount);
	f = (PerformanceCount - counter) / frequency;
	counter += frequency * f;
	n = (int)f;
	return n;
}

void S_TimeReset() {
	QueryPerformanceCounter((LARGE_INTEGER*)&counter);
}

int S_TimeInit() {
	int pfq;

	Log(__func__, "TIME_Init");

	if(!QueryPerformanceFrequency((LARGE_INTEGER*)&pfq)) {
		return 0;
	}

	frequency = pfq / 60;
	S_TimeReset();
	return 1;
}
