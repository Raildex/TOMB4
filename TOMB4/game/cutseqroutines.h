#ifndef CUTSEQ_ROUTINES_INCLUDED
#define CUTSEQ_ROUTINES_INCLUDED
struct CUTSEQ_ROUTINES
{
	void(*init_func)();
	void(*control_func)();
	void(*end_func)();
};
#endif