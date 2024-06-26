#include "specific/winapp.h"
#include <stdbool.h>
bool WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex);
void WinProcessCommandLine(LPSTR cmd);
void WinClose();
float WinFrameRate();
void WinDisplayString(long x, long y, char* string, ...);
void WinProcMsg();
void WinProcessCommands(long cmd);
void ClearSurfaces();
bool WinRegisterWindow(HINSTANCE hinstance);
bool WinCreateWindow();
void WinSetStyle(bool fullscreen, unsigned long* set);

extern WINAPP App;
extern char* cutseqpakPtr;
extern long resChangeCounter;
