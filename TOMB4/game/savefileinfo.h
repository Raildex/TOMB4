#ifndef SAVEFILE_INFO_INCLUDED
#define SAVEFILE_INFO_INCLUDED
typedef struct SAVEFILE_INFO {
	char name[75];
	char valid;
	short hours;
	short minutes;
	short seconds;
	short days;
	long num;
} SAVEFILE_INFO;
#endif