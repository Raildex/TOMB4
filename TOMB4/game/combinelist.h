#ifndef TOMB4_GAME_COMBINELIST_H
#define TOMB4_GAME_COMBINELIST_H
typedef struct COMBINELIST {
	void (*combine_routine)(long flag);
	short item1;
	short item2;
	short combined_item;
} COMBINELIST;
#endif // TOMB4_GAME_COMBINELIST_H
