#ifndef TOMB4_GAME_AHMET_H
#define TOMB4_GAME_AHMET_H
typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;
void ScalesCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ScalesControl(short item_number);
void InitialiseAhmet(short item_number);
void AhmetControl(short item_number);


#endif// TOMB4_GAME_AHMET_H
