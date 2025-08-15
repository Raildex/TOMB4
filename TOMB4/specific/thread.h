#ifndef TOMB4_SPECIFIC_THREAD_H
#define TOMB4_SPECIFIC_THREAD_H
typedef struct THREAD THREAD;
typedef int (*THREADFUNC)(void* arg);
int S_CreateThread(THREAD** output, THREADFUNC func, void* arg);
int S_RunThread(THREAD* thrd);
int S_PauseThread(THREAD* thrd);
int S_WaitThread(THREAD* thrd);

#endif// TOMB4_SPECIFIC_THREAD_H
