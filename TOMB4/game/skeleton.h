#ifndef TOMB4_GAME_SKELETON_H
#define TOMB4_GAME_SKELETON_H

typedef struct ITEM_INFO ITEM_INFO;


void TriggerRiseEffect(ITEM_INFO* item);
void InitialiseSkeleton(short item_number);
void SkeletonControl(short item_number);


#endif// TOMB4_GAME_SKELETON_H
