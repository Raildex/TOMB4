#ifndef TOMB4_GAME_ITEMS_H
#define TOMB4_GAME_ITEMS_H
enum no_item {
	NO_ITEM = -1,
};

void InitialiseItemArray(short num, long count);
void KillItem(short item_num);
short CreateItem();
void InitialiseItem(short item_num);
void RemoveActiveItem(short item_num);
void RemoveDrawnItem(short item_num);
void AddActiveItem(short item_num);
void ItemNewRoom(short item_num, short room_num);
void InitialiseFXArray();
short CreateEffect(short room_num);
void KillEffect(short fx_num);
void EffectNewRoom(short fx_num, short room_num);

extern short next_fx_active;
extern short next_item_active;


#endif// TOMB4_GAME_ITEMS_H
