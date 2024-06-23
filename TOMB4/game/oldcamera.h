#ifndef OLD_CAMERA_INCLUDED
#define OLD_CAMERA_INCLUDED
struct OLD_CAMERA
{
	short current_anim_state;
	short goal_anim_state;
	long target_distance;
	short target_angle;
	short target_elevation;
	PHD_3DPOS pos;
	PHD_3DPOS pos2;
	PHD_VECTOR t;
};
#endif