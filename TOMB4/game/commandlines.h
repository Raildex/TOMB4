#ifndef COMMANDLINES_INCLUDED
#define COMMANDLINES_INCLUDED
typedef struct COMMANDLINES {
	char command[20];
	bool needs_parameter;
	void (*code)(char*);
	char parameter[20];
} COMMANDLINES;
#endif