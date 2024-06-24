#ifndef LOCUST_STRUCT_INCLUDED
#define LOCUST_STRUCT_INCLUDED
struct LOCUST_STRUCT {
	PHD_3DPOS pos;
	short room_number;
	short speed;
	short Counter;
	short LaraTarget;
	char XTarget;
	char ZTarget;
	unsigned char On;
	unsigned char flags;
};
#endif