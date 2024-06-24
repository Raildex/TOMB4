#ifndef TOMB4_GAME_COMBINELIST_H
#define TOMB4_GAME_COMBINELIST_H
struct COMBINELIST {
	void (*combine_routine)(long flag);
	short item1;
	short item2;
	short combined_item;
};
#endif // TOMB4_GAME_COMBINELIST_H
