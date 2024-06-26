#ifndef TOMB4_GAME_AIINFO_H
#define TOMB4_GAME_AIINFO_H
typedef struct AI_INFO {
	short zone_number;
	short enemy_zone;
	long distance;
	long ahead;
	long bite;
	short angle;
	short x_angle;
	short enemy_facing;
} AI_INFO;
#endif // TOMB4_GAME_AIINFO_H
