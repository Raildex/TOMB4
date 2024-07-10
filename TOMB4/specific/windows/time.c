
#include "specific/function_stubs.h"
#include <windows.h>
static long long counter, frequency;

long S_Sync() {
	long long PerformanceCount, f;
	long n;

	QueryPerformanceCounter((LARGE_INTEGER*)&PerformanceCount);
	f = (PerformanceCount - counter) / frequency;
	counter += frequency * f;
	n = (long)f;
	return n;
}

void S_TimeReset() {
	QueryPerformanceCounter((LARGE_INTEGER*)&counter);
}

long S_TimeInit() {
	long long pfq;

	Log(__func__, "TIME_Init");

	if(!QueryPerformanceFrequency((LARGE_INTEGER*)&pfq)) {
		return 0;
	}

	frequency = pfq / 60;
	S_TimeReset();
	return 1;
}
