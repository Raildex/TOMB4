#ifndef TOMB4_GAME_DEATHSLD_H
#define TOMB4_GAME_DEATHSLD_H

typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;

void InitialiseDeathSlide(short item_number);
void DeathSlideCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ControlDeathSlide(short item_number);


#endif// TOMB4_GAME_DEATHSLD_H
