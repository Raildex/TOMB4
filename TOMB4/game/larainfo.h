#ifndef TOMB4_GAME_LARAINFO_H
#define TOMB4_GAME_LARAINFO_H
#include "laraarm.h"
#include "phdvector.h"

struct CREATURE_INFO;
struct FX_INFO;
struct ITEM_INFO;
#pragma pack(push,1)
struct LARA_INFO
{
	short item_number;
	short gun_status;
	short gun_type;
	short request_gun_type;
	short last_gun_type;
	short calc_fallspeed;
	short water_status;
	short climb_status;
	short pose_count;
	short hit_frame;
	short hit_direction;
	short air;
	short dive_count;
	short death_count;
	short current_active;
	short current_xvel;
	short current_yvel;
	short current_zvel;
	short spaz_effect_count;
	short flare_age;
	short vehicle;
	short weapon_item;
	short back_gun;
	short flare_frame;
	short poisoned;
	short dpoisoned;
	unsigned char electric;
	unsigned char wet[15];
	unsigned short flare_control_left : 1;
	unsigned short Unused1 : 1;
	unsigned short look : 1;
	unsigned short burn : 1;
	unsigned short keep_ducked : 1;
	unsigned short IsMoving : 1;
	unsigned short CanMonkeySwing : 1;
	unsigned short Unused2 : 1;
	unsigned short OnBeetleFloor : 1;
	unsigned short BurnGreen : 1;
	unsigned short IsDucked : 1;
	unsigned short has_fired : 1;
	unsigned short Busy : 1;
	unsigned short LitTorch : 1;
	unsigned short IsClimbing : 1;
	unsigned short Fired : 1;
	long water_surface_dist;
	PHD_VECTOR last_pos;
	FX_INFO* spaz_effect;
	long mesh_effects;
	short* mesh_ptrs[15];
	ITEM_INFO* target;
	short target_angles[2];
	short turn_rate;
	short move_angle;
	short head_y_rot;
	short head_x_rot;
	short head_z_rot;
	short torso_y_rot;
	short torso_x_rot;
	short torso_z_rot;
	LARA_ARM left_arm;
	LARA_ARM right_arm;
	unsigned short holster;
	CREATURE_INFO* creature;
	long CornerX;
	long CornerZ;
	char RopeSegment;
	char RopeDirection;
	short RopeArcFront;
	short RopeArcBack;
	short RopeLastX;
	short RopeMaxXForward;
	short RopeMaxXBackward;
	long RopeDFrame;
	long RopeFrame;
	unsigned short RopeFrameRate;
	unsigned short RopeY;
	long RopePtr;
	void* GeneralPtr;
	long RopeOffset;
	unsigned long RopeDownVel;
	char RopeFlag;
	char MoveCount;
	long RopeCount;
	char pistols_type_carried;
	char uzis_type_carried;
	char shotgun_type_carried;
	char crossbow_type_carried;
	char grenade_type_carried;
	char sixshooter_type_carried;
	char lasersight;
	char binoculars;
	char crowbar;
	char mechanical_scarab;
	unsigned char small_water_skin;
	unsigned char big_water_skin;
	char examine1;
	char examine2;
	char examine3;
	char puzzleitems[12];
	unsigned short puzzleitemscombo;
	unsigned short keyitems;
	unsigned short keyitemscombo;
	unsigned short pickupitems;
	unsigned short pickupitemscombo;
	short questitems;
	short num_small_medipack;
	short num_large_medipack;
	short num_flares;
	short num_pistols_ammo;
	short num_uzi_ammo;
	short num_revolver_ammo;
	short num_shotgun_ammo1;
	short num_shotgun_ammo2;
	short num_grenade_ammo1;
	short num_grenade_ammo2;
	short num_grenade_ammo3;
	short num_crossbow_ammo1;
	short num_crossbow_ammo2;
	short num_crossbow_ammo3;
	char beetle_uses;
	char blindTimer;
	char location;
	char highest_location;
	char locationPad;
};
#pragma pack(pop)
#endif// TOMB4_GAME_LARAINFO_H
