#ifndef TOMB4_GAME_CUTSEQROUTINES_H
#define TOMB4_GAME_CUTSEQROUTINES_H
typedef struct CUTSEQ_ROUTINES {
	void (*init_func)();
	void (*control_func)();
	void (*end_func)();
} CUTSEQ_ROUTINES;
#endif // TOMB4_GAME_CUTSEQROUTINES_H
