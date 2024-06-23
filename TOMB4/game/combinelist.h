#ifndef COMBINELIST_INCLUDED
#define COMBINELIST_INCLUDED
struct COMBINELIST
{
	void(*combine_routine)(long flag);
	short item1;
	short item2;
	short combined_item;
};
#endif