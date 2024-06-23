#ifndef SAVEFILE_INFO_INCLUDED
#define SAVEFILE_INFO_INCLUDED
struct SAVEFILE_INFO
{
	char name[75];
	char valid;
	short hours;
	short minutes;
	short seconds;
	short days;
	long num;
};
#endif