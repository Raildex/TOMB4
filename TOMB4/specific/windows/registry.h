#ifndef TOMB4_SPECIFIC_REGISTRY_H
#define TOMB4_SPECIFIC_REGISTRY_H
#include <windows.h>

char REG_OpenKey(LPCSTR lpSubKey);
char OpenRegistry(LPCSTR SubKeyName);
void REG_CloseKey();
void CloseRegistry();
void REG_WriteLong(char* SubKeyName, int value);
void REG_WriteString(char* SubKeyName, char* string, int length);
void REG_WriteFloat(char* SubKeyName, float value);
char REG_ReadLongDefault(char* SubKeyName, long* value, int defaultValue);
char REG_ReadStringDefault(char* SubKeyName, char* value, int length, char* defaultValue);
char REG_ReadFloatDefault(char* SubKeyName, float* value, float defaultValue);
char LoadSettings();
void SaveSettings();
char SaveSetup(HWND hDlg);
char REG_KeyWasCreated();


#endif// TOMB4_SPECIFIC_REGISTRY_H
