#ifndef TOMB4_SPECIFIC_THREAD_H
#define TOMB4_SPECIFIC_THREAD_H
typedef struct THREAD THREAD;
typedef int (*THREADFUNC)(void* arg);
long S_CreateThread(THREAD** output, THREADFUNC func, void* arg);
long S_RunThread(THREAD* thrd);
long S_PauseThread(THREAD* thrd);
long S_WaitThread(THREAD* thrd);

#endif// TOMB4_SPECIFIC_THREAD_H
