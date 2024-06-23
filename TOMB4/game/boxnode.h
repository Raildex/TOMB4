#ifndef BOX_NODE_INCLUDED
#define BOX_NODE_INCLUDED
struct BOX_NODE
{
	short exit_box;
	unsigned short search_number;
	short next_expansion;
	short box_number;
};
#endif