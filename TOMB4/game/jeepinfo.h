#ifndef TOMB4_GAME_JEEPINFO_H
#define TOMB4_GAME_JEEPINFO_H
typedef struct JEEPINFO {
	short right_front_wheelrot;
	short left_front_wheelrot;
	short left_back_wheelrot;
	short right_back_wheelrot;
	long velocity;
	long unused1;
	long pitch1;
	long turn_rate;
	long camera_angle;
	short move_angle;
	short extra_rotation;
	short rot_thing;
	long pitch2;
	short flags;
	short unused2;
	short gear;
} JEEPINFO;
#endif // TOMB4_GAME_JEEPINFO_H
