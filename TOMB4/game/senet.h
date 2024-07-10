#ifndef TOMB4_GAME_SENET_H
#define TOMB4_GAME_SENET_H

typedef struct ITEM_INFO ITEM_INFO;
typedef struct COLL_INFO COLL_INFO;


void InitialiseSenet(short item_number);
void MakeMove(long piece, long displacement);
void SenetControl(short item_number);
long CheckSenetWinner(long won);
void InitialiseGameStix(short item_number);
void ThrowSticks(ITEM_INFO* item);
void GameStixControl(short item_number);
void GameStixCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void ShockwaveExplosion(ITEM_INFO* item, unsigned long col, long speed);
void ControlGodHead(short item_number);
void DrawGodHead(ITEM_INFO* item);

extern long SenetTargetX;
extern long SenetTargetZ;
extern short senet_item[6];
extern char piece_moving;
extern char last_throw;
extern char senet_board[17];
extern char senet_piece[6];


#endif// TOMB4_GAME_SENET_H
