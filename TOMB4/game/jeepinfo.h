#ifndef TOMB4_GAME_JEEPINFO_H
#define TOMB4_GAME_JEEPINFO_H
typedef struct JEEPINFO {
	short right_front_wheelrot;
	short left_front_wheelrot;
	short left_back_wheelrot;
	short right_back_wheelrot;
	int velocity;
	int unused1;
	int pitch1;
	int turn_rate;
	int camera_angle;
	short move_angle;
	short extra_rotation;
	short rot_thing;
	int pitch2;
	short flags;
	short unused2;
	short gear;
} JEEPINFO;
#endif // TOMB4_GAME_JEEPINFO_H
