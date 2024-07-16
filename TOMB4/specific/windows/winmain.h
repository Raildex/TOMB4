#include "specific/windows/winapp.h"
long WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex);
void WinProcessCommandLine(LPSTR cmd);
void WinClose();
float WinFrameRate();
void WinDisplayString(long x, long y, char* string, ...);
void WinProcMsg();
void WinProcessCommands(long cmd);
void ClearSurfaces();
long WinRegisterWindow(HINSTANCE hinstance);
long WinCreateWindow();
void WinSetStyle(long fullscreen, unsigned long* set);

extern WINAPP App;
extern char* cutseqpakPtr;
extern long resChangeCounter;
