#ifndef TOMB4_GAME_WRAITH_H
#define TOMB4_GAME_WRAITH_H


void InitialiseWraith(short item_number);
void TriggerWraithFlame(long x, long y, long z, short xv, short yv, short zv, long objnum);
void TriggerWraithEffect(long x, long y, long z, short vel, long objnum);
void WraithControl(short item_number);


#endif// TOMB4_GAME_WRAITH_H
