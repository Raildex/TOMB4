#ifndef SPECIFIC_WINDOWS_CMDLINE_H
#define SPECIFIC_WINDOWS_CMDLINE_H
#include <windows.h>
void CLSetup(char* cmd);
void CLNoFMV(char* cmd);
void InitDSDevice(HWND dlg, HWND hwnd);
void InitTFormats(HWND dlg, HWND hwnd);
void InitResolution(HWND dlg, HWND hwnd, int resetvms);
void InitD3DDevice(HWND dlg, HWND hwnd);
void InitDDDevice(HWND dlg, HWND hwnd);
char* MapASCIIToANSI(char* s, char* d);
INT_PTR WINAPI DXSetupDlgProc(HWND dlg, UINT message, WPARAM wParam, LPARAM lParam);
int DXSetupDialog();

extern int start_setup;
extern int fmvs_disabled;


#endif// SPECIFIC_WINDOWS_CMDLINE_H
