#ifndef TOMB4_GAME_BIKEINFO_H
#define TOMB4_GAME_BIKEINFO_H
typedef struct BIKEINFO {
	short right_front_wheelrot;
	short right_back_wheelrot;
	long left_wheelrot;
	long velocity;
	long unused1;
	long pitch1;
	short move_angle;
	short extra_rotation;
	short rot_thing;
	long bike_turn;
	long pitch2;
	short flags;
	short light_intensity;
} BIKEINFO;
#endif // TOMB4_GAME_BIKEINFO_H
