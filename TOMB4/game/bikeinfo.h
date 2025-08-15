#ifndef TOMB4_GAME_BIKEINFO_H
#define TOMB4_GAME_BIKEINFO_H
typedef struct BIKEINFO {
	short right_front_wheelrot;
	short right_back_wheelrot;
	int left_wheelrot;
	int velocity;
	int unused1;
	int pitch1;
	short move_angle;
	short extra_rotation;
	short rot_thing;
	int bike_turn;
	int pitch2;
	short flags;
	short light_intensity;
} BIKEINFO;
#endif// TOMB4_GAME_BIKEINFO_H
