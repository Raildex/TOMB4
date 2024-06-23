#ifndef COMMANDLINES_INCLUDED
#define COMMANDLINES_INCLUDED
struct COMMANDLINES
{
	char command[20];
	bool needs_parameter;
	void (*code)(char*);
	char parameter[20];
};
#endif