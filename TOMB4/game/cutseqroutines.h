#ifndef TOMB4_GAME_CUTSEQROUTINES_H
#define TOMB4_GAME_CUTSEQROUTINES_H
struct CUTSEQ_ROUTINES {
	void (*init_func)();
	void (*control_func)();
	void (*end_func)();
};
#endif // TOMB4_GAME_CUTSEQROUTINES_H
