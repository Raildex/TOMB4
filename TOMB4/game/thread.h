#ifndef THREAD_INCLUDED
#define THREAD_INCLUDED

struct THREAD
{
	volatile long active;
	long unk;
	volatile long ended;
	unsigned long handle;
	unsigned long address;
};
#endif