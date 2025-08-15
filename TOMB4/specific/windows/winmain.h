#include "specific/windows/winapp.h"
int WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex);
void WinProcessCommandLine(LPSTR cmd);
void WinClose();
float WinFrameRate();
void WinDisplayString(int x, int y, char* string, ...);
void WinProcMsg();
void WinProcessCommands(int cmd);
void ClearSurfaces();
int WinRegisterWindow(HINSTANCE hinstance);
int WinCreateWindow();
void WinSetStyle(int fullscreen, unsigned long* set);

extern WINAPP App;
extern char* cutseqpakPtr;
extern int resChangeCounter;
