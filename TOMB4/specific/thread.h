#ifndef THREAD_INCLUDED
#define THREAD_INCLUDED

typedef struct THREAD {
	volatile long active;
	long unk;
	volatile long ended;
	unsigned long handle;
	unsigned long address;
} THREAD;
#endif