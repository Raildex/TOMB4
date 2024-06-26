#ifndef TOMB4_GAME_BOXNODE_H
#define TOMB4_GAME_BOXNODE_H
typedef struct BOX_NODE {
	short exit_box;
	unsigned short search_number;
	short next_expansion;
	short box_number;
} BOX_NODE;
#endif // TOMB4_GAME_BOXNODE_H
