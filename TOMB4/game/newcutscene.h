#ifndef NEW_CUTSCENE_INCLUDED
#define NEW_CUTSCENE_INCLUDED
struct NEW_CUTSCENE
{
	short numactors;
	short numframes;
	long orgx;
	long orgy;
	long orgz;
	long audio_track;
	long camera_offset;
	ACTORME actor_data[10];
};
#endif