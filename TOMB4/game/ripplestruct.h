#ifndef TOMB4_GAME_RIPPLESTRUCT_H
#define TOMB4_GAME_RIPPLESTRUCT_H
enum ripple_flags {
	ripple_on = 1 << 0,
};

typedef struct RIPPLE_STRUCT {
	long x;
	long y;
	long z;
	enum ripple_flags flags;
	unsigned char life;
	unsigned char size;
	unsigned char init;
} RIPPLE_STRUCT;


#endif // TOMB4_GAME_RIPPLESTRUCT_H
