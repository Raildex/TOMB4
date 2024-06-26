#pragma once
#include <windows.h>
#include <stdbool.h>

bool REG_OpenKey(LPCSTR lpSubKey);
bool OpenRegistry(LPCSTR SubKeyName);
void REG_CloseKey();
void CloseRegistry();
void REG_WriteLong(char* SubKeyName, long value);
void REG_WriteBool(char* SubKeyName, bool value);
void REG_WriteString(char* SubKeyName, char* string, long length);
void REG_WriteFloat(char* SubKeyName, float value);
bool REG_ReadLongDefault(char* SubKeyName, long* value, long defaultValue);
bool REG_ReadBoolDefault(char* SubKeyName, bool* value, bool defaultValue);
bool REG_ReadStringDefault(char* SubKeyName, char* value, long length, char* defaultValue);
bool REG_ReadFloatDefault(char* SubKeyName, float* value, float defaultValue);
bool LoadSettings();
void SaveSettings();
bool SaveSetup(HWND hDlg);
bool REG_KeyWasCreated();
