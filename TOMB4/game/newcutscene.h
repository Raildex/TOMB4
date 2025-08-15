#ifndef NEW_CUTSCENE_INCLUDED
#define NEW_CUTSCENE_INCLUDED
typedef struct NEW_CUTSCENE {
	short numactors;
	short numframes;
	int orgx;
	int orgy;
	int orgz;
	int audio_track;
	int camera_offset;
	ACTORME actor_data[10];
} NEW_CUTSCENE;
#endif
