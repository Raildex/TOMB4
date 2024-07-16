#ifndef TOMB4_GAME_BOX_H
#define TOMB4_GAME_BOX_H
#include "game/targettype.h"
enum no_box {
	NO_BOX = 2047
};

typedef struct ITEM_INFO ITEM_INFO;
typedef struct AI_INFO AI_INFO;
typedef struct LOT_INFO LOT_INFO;
typedef struct PHD_VECTOR PHD_VECTOR;
typedef struct BITE_INFO BITE_INFO;
typedef struct CREATURE_INFO CREATURE_INFO;
typedef struct PHD_3DPOS PHD_3DPOS;
typedef struct BOX_INFO BOX_INFO;
typedef short (*creature_effect_routine)(long x, long y, long z, short speed_or_damage, short yRot, PHD_VECTOR direction, short room);
void CreatureDie(short item_number, long explode);
void InitialiseCreature(short item_number);
long CreatureActive(short item_number);
void CreatureAIInfo(ITEM_INFO* item, AI_INFO* info);
long SearchLOT(LOT_INFO* LOT, long expansion);
long UpdateLOT(LOT_INFO* LOT, long expansion);
void TargetBox(LOT_INFO* LOT, short box_number);
long EscapeBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number);
long ValidBox(ITEM_INFO* item, short zone_number, short box_number);
long StalkBox(ITEM_INFO* item, ITEM_INFO* enemy, short box_number);
target_type CalculateTarget(PHD_VECTOR* target, ITEM_INFO* item, LOT_INFO* LOT);
void CreatureMood(ITEM_INFO* item, AI_INFO* info, long violent);
void GetCreatureMood(ITEM_INFO* item, AI_INFO* info, long violent);
long CreatureCreature(short item_number);
long BadFloor(long x, long y, long z, long box_height, long next_height, short room_number, LOT_INFO* LOT);
long CreatureAnimation(short item_number, short angle, short tilt);
short CreatureTurn(ITEM_INFO* item, short maximum_turn);
void CreatureTilt(ITEM_INFO* item, short angle);
void CreatureJoint(ITEM_INFO* item, short joint, short required);
void CreatureFloat(short item_number);
void CreatureUnderwater(ITEM_INFO* item, long depth);
short CreatureEffect(ITEM_INFO* item, BITE_INFO* bite, creature_effect_routine generate);
short CreatureEffectT(ITEM_INFO* item, BITE_INFO* bite, short damage, short angle, creature_effect_routine generate);
long CreatureVault(short item_number, short angle, long vault, long shift);
void CreatureKill(ITEM_INFO* item, short kill_anim, short kill_state, short lara_anim);
void AlertAllGuards(short item_number);
void AlertNearbyGuards(ITEM_INFO* item);
short AIGuard(CREATURE_INFO* creature);
void FindAITargetObject(CREATURE_INFO* creature, short obj_num);
void GetAITarget(CREATURE_INFO* creature);
short SameZone(CREATURE_INFO* creature, ITEM_INFO* target_item);
void CreatureYRot(PHD_3DPOS* srcpos, short angle, short angadd);
long MoveCreature3DPos(PHD_3DPOS* srcpos, PHD_3DPOS* destpos, long velocity, short angdif, long angadd);



#endif// TOMB4_GAME_BOX_H
