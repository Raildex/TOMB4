#ifndef TOMB4_GAME_LARAFLAR_H
#define TOMB4_GAME_LARAFLAR_H

typedef struct ITEM_INFO ITEM_INFO;
typedef struct PHD_VECTOR PHD_VECTOR;


void DrawFlareInAir(ITEM_INFO* item);
void draw_flare_meshes();
void undraw_flare_meshes();
int DoFlareLight(PHD_VECTOR* pos, int flare_age);
void DoFlareInHand(int flare_age);
void CreateFlare(short object, int thrown);
void set_flare_arm(int frame);
void ready_flare();
void draw_flare();
void undraw_flare();
void FlareControl(short item_number);


#endif// TOMB4_GAME_LARAFLAR_H
