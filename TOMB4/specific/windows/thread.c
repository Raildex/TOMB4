#include "specific/thread.h"
#include <windows.h>
#include <stdlib.h>
typedef struct THREAD {
	ULONG Win32Handle;
	HANDLE ThreadHandle;
} THREAD;

typedef struct WIN32PAYLOAD {
	void* arg;
	THREADFUNC func;
}WIN32PAYLOAD;

ULONG WINAPI Win32ThreadFunc(void* arg) {
	WIN32PAYLOAD payload = *(WIN32PAYLOAD*)(arg);
	int status = payload.func(payload.func);
	return status;
}

long S_CreateThread(THREAD** output,THREADFUNC func, void* arg) {
	THREAD* thrd = calloc(1,sizeof(THREAD));
	WIN32PAYLOAD payload;
	payload.arg = arg;
	payload.func = func;
	HANDLE handle = CreateThread(NULL,0,Win32ThreadFunc,&payload,CREATE_SUSPENDED,&thrd->Win32Handle);
	if(handle) {
		thrd->ThreadHandle = handle;
		*output = thrd;
		return 1;
	}
	*output = NULL;
	return 0;
}

long S_RunThread(THREAD* thrd) {
	DWORD result = ResumeThread(thrd->ThreadHandle);
	if(result == -1) {
		return 0;
	}
	return 1;
}

long S_WaitThread(THREAD* thrd) {
	DWORD result = WaitForMultipleObjects(1, thrd->ThreadHandle, 1, INFINITE);
	if(result == WAIT_FAILED || result == WAIT_TIMEOUT) {
		return 0;
	}
	return 1;
}

long S_PauseThread(THREAD *thrd) {
	DWORD result = SuspendThread(thrd->ThreadHandle);
	if(result == -1) {
		return 0;
	}
	return 1;
}