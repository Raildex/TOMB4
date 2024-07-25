#ifndef TOMB4_GAME_WEAPONINFO_H
#define TOMB4_GAME_WEAPONINFO_H
typedef struct WEAPON_INFO {
	short lock_angles[4];
	short left_angles[4];
	short right_angles[4];
	short aim_speed;
	short shot_accuracy;
	short gun_height;
	short target_dist;
	char damage;
	char recoil_frame;
	char flash_time;
	char draw_frame;
	short sample_num;
} WEAPON_INFO;
#endif// TOMB4_GAME_WEAPONINFO_H
