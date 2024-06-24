#ifndef TOMB4_GAME_OLDCAMERA_H
#define TOMB4_GAME_OLDCAMERA_H
struct OLD_CAMERA {
	short current_anim_state;
	short goal_anim_state;
	long target_distance;
	short target_angle;
	short target_elevation;
	PHD_3DPOS pos;
	PHD_3DPOS pos2;
	PHD_VECTOR t;
};
#endif // TOMB4_GAME_OLDCAMERA_H
