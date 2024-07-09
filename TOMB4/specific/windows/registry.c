
#include "specific/registry.h"
#include "specific/loadsave.h"
#include "specific/cmdline.h"
#include "specific/input.h"
#include "specific/windows/winmain.h"
#include "specific/windows/dxflags.h"
#include <string.h>
#include <stdio.h>

static HKEY phkResult;
static DWORD dwDisposition;
static long REG_Setup;

char REG_OpenKey(LPCSTR lpSubKey) {
	return RegCreateKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, (CHAR*)"", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &phkResult, &dwDisposition) == ERROR_SUCCESS;
}

char OpenRegistry(LPCSTR SubKeyName) {
	char buffer[256];

	if(!SubKeyName)
		return REG_OpenKey("Software\\Core Design\\Tomb Raider IV");

	sprintf(buffer, "%s\\%s", "Software\\Core Design\\Tomb Raider IV", SubKeyName);
	return REG_OpenKey(buffer);
}

void REG_CloseKey() {
	RegCloseKey(phkResult);
}

void CloseRegistry() {
	REG_CloseKey();
}

void REG_WriteLong(char* SubKeyName, long value) {
	RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE*)&value, sizeof(value));
}

void REG_WriteString(char* SubKeyName, char* string, long length) {
	long checkLength;

	if(string) {
		if(length < 0)
			checkLength = strlen(string);
		else
			checkLength = length;

		RegSetValueEx(phkResult, SubKeyName, 0, REG_SZ, (CONST BYTE*)string, checkLength + 1);
	} else
		RegDeleteValue(phkResult, SubKeyName);
}

void REG_WriteFloat(char* SubKeyName, float value) {
	long length;
	char buf[64];

	length = sprintf(buf, "%.5f", value);
	REG_WriteString(SubKeyName, buf, length);
}

char REG_ReadLongDefault(char* SubKeyName, long* value, long defaultValue) {
	unsigned long type;
	unsigned long cbData;

	cbData = sizeof(*value);
	unsigned char buffer[sizeof(*value)];

	if(RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)&buffer[0], &cbData) == ERROR_SUCCESS && type == REG_DWORD && cbData == sizeof(*value)) {
		long readValue;
		memcpy(&readValue, buffer, sizeof(*value));
		*value = readValue;
		return 1;
	}

	REG_WriteLong(SubKeyName, defaultValue);
	*value = defaultValue;
	return 0;
}

char REG_ReadString(char* SubKeyName, char* value, long length, char* defaultValue) {
	unsigned long type;
	unsigned long cbData;
	long len;

	cbData = length;

	if(RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)value, (LPDWORD)&cbData) == ERROR_SUCCESS && type == REG_SZ)
		return 1;

	if(defaultValue) {
		REG_WriteString(SubKeyName, defaultValue, -1);
		len = strlen(defaultValue) + 1;

		if(len > length) {
			len = length - 1;
			value[len] = 0;
		}

		memcpy(value, defaultValue, len);
	} else
		RegDeleteValue(phkResult, SubKeyName);

	return 0;
}

char REG_ReadFloatDefault(char* SubKeyName, float* value, float defaultValue) {
	char buf[64];

	if(REG_ReadString(SubKeyName, buf, sizeof(buf), 0)) {
		*value = (float)atof(buf);
		return 1;
	}

	REG_WriteFloat(SubKeyName, defaultValue);
	*value = defaultValue;
	return 0;
}

char LoadSettings() {
	long key;
	long val;

	if(!OpenRegistry("System"))
		return 0;

	REG_ReadLongDefault((char*)"Setup", &REG_Setup, 0);

	if(REG_Setup) {
		App.TextureSize = 256;
		App.BumpMapSize = 256;
		App.StartFlags = DXF_FPUSETUP;

		REG_ReadLongDefault("DD", &App.DXInfo.nDD, 0);
		REG_ReadLongDefault("D3D", &App.DXInfo.nD3D, 0);
		REG_ReadLongDefault("VMode", &App.DXInfo.nDisplayMode, 0);
		REG_ReadLongDefault("TFormat", &App.DXInfo.nTexture, 0);
		REG_ReadLongDefault("DS", &App.DXInfo.nDS, 0);
		REG_ReadLongDefault("BumpMap", &App.BumpMapping, 1);
		REG_ReadLongDefault("Filter", &App.Filtering, 1);
		REG_ReadLongDefault("DisableSound", &App.SoundDisabled, 0);
		REG_ReadLongDefault("Volumetric", &App.Volumetric, 1);
		REG_ReadLongDefault("NoFMV", &fmvs_disabled, 0);

		REG_ReadLongDefault("TextLow", &val, 0);

		if(val)
			App.TextureSize = 128;

		REG_ReadLongDefault("BumpLow", &val, 0);

		if(val)
			App.BumpMapSize = 128;

		REG_ReadLongDefault("HardWare", &val, 1);

		if(val)
			App.StartFlags |= DXF_ZBUFFER | DXF_HWR;

		REG_ReadLongDefault("Window", &val, 0);

		if(val)
			App.StartFlags |= DXF_WINDOWED;
		else
			App.StartFlags |= DXF_FULLSCREEN;
	}

	CloseRegistry();

	OpenRegistry("Game");

	REG_ReadLongDefault("Key0", &key, layout[0][0]);
	layout[1][0] = (short)key;

	REG_ReadLongDefault("Key1", &key, layout[0][1]);
	layout[1][1] = (short)key;

	REG_ReadLongDefault("Key2", &key, layout[0][2]);
	layout[1][2] = (short)key;

	REG_ReadLongDefault("Key3", &key, layout[0][3]);
	layout[1][3] = (short)key;

	REG_ReadLongDefault("Key4", &key, layout[0][4]);
	layout[1][4] = (short)key;

	REG_ReadLongDefault("Key5", &key, layout[0][5]);
	layout[1][5] = (short)key;

	REG_ReadLongDefault("Key6", &key, layout[0][6]);
	layout[1][6] = (short)key;

	REG_ReadLongDefault("Key7", &key, layout[0][7]);
	layout[1][7] = (short)key;

	REG_ReadLongDefault("Key8", &key, layout[0][8]);
	layout[1][8] = (short)key;

	REG_ReadLongDefault("Key9", &key, layout[0][9]);
	layout[1][9] = (short)key;

	REG_ReadLongDefault("Key10", &key, layout[0][10]);
	layout[1][10] = (short)key;

	REG_ReadLongDefault("Key11", &key, layout[0][11]);
	layout[1][11] = (short)key;

	REG_ReadLongDefault("Key12", &key, layout[0][12]);
	layout[1][12] = (short)key;

	REG_ReadLongDefault("Key13", &key, layout[0][13]);
	layout[1][13] = (short)key;

	REG_ReadLongDefault("Key14", &key, layout[0][14]);
	layout[1][14] = (short)key;

	REG_ReadLongDefault("Key15", &key, layout[0][15]);
	layout[1][15] = (short)key;

	REG_ReadLongDefault("Key16", &key, layout[0][16]);
	layout[1][16] = (short)key;

	REG_ReadLongDefault("Key17", &key, layout[0][17]);
	layout[1][17] = (short)key;

	REG_ReadLongDefault("MusicVolume", &MusicVolume, 80);
	REG_ReadLongDefault("SFXVolume", &SFXVolume, 90);
	REG_ReadLongDefault("ControlMethod", &ControlMethod, 0);
	REG_ReadLongDefault("SoundQuality", &SoundQuality, 1);
	REG_ReadLongDefault("AutoTarget", &App.AutoTarget, 1);
	REG_ReadLongDefault("WindowX", &App.dx.rScreen.left, 0);
	REG_ReadLongDefault("WindowY", &App.dx.rScreen.top, 0);

	CloseRegistry();
	CheckKeyConflicts();
	return (char)REG_Setup;
}

void SaveSettings() {
	OpenRegistry("Game");
	REG_WriteLong("Key0", layout[1][0]);
	REG_WriteLong("Key1", layout[1][1]);
	REG_WriteLong("Key2", layout[1][2]);
	REG_WriteLong("Key3", layout[1][3]);
	REG_WriteLong("Key4", layout[1][4]);
	REG_WriteLong("Key5", layout[1][5]);
	REG_WriteLong("Key6", layout[1][6]);
	REG_WriteLong("Key7", layout[1][7]);
	REG_WriteLong("Key8", layout[1][8]);
	REG_WriteLong("Key9", layout[1][9]);
	REG_WriteLong("Key10", layout[1][10]);
	REG_WriteLong("Key11", layout[1][11]);
	REG_WriteLong("Key12", layout[1][12]);
	REG_WriteLong("Key13", layout[1][13]);
	REG_WriteLong("Key14", layout[1][14]);
	REG_WriteLong("Key15", layout[1][15]);
	REG_WriteLong("Key16", layout[1][16]);
	REG_WriteLong("Key17", layout[1][17]);
	REG_WriteLong("ControlMethod", ControlMethod);
	REG_WriteLong("MusicVolume", MusicVolume);
	REG_WriteLong("SFXVolume", SFXVolume);
	REG_WriteLong("SoundQuality", SoundQuality);
	REG_WriteLong("AutoTarget", App.AutoTarget);
	REG_WriteLong("WindowX", App.dx.rScreen.left);
	REG_WriteLong("WindowY", App.dx.rScreen.top);
	CloseRegistry();

	OpenRegistry("System");
	REG_WriteLong("VMode", App.DXInfo.nDisplayMode);
	REG_WriteLong("Window", (App.dx.Flags & DXF_WINDOWED) != 0);
	CloseRegistry();
}

char SaveSetup(HWND hDlg) {
	OpenRegistry("System");

	REG_WriteLong("DD", SendMessage(GetDlgItem(hDlg, 1000), CB_GETCURSEL, 0, 0));
	REG_WriteLong("D3D", SendMessage(GetDlgItem(hDlg, 1003), CB_GETCURSEL, 0, 0));
	REG_WriteLong("VMode", SendMessage(GetDlgItem(hDlg, 1004), CB_GETITEMDATA, SendMessage(GetDlgItem(hDlg, 1004), CB_GETCURSEL, 0, 0), 0));
	REG_WriteLong("DS", SendMessage(GetDlgItem(hDlg, 1005), CB_GETCURSEL, 0, 0));
	REG_WriteLong("TFormat", SendMessage(GetDlgItem(hDlg, 1006), CB_GETCURSEL, 0, 0));

	REG_WriteLong("Filter", SendMessage(GetDlgItem(hDlg, 1012), BM_GETCHECK, 0, 0));
	REG_WriteLong("BumpMap", SendMessage(GetDlgItem(hDlg, 1016), BM_GETCHECK, 0, 0));
	REG_WriteLong("HardWare", SendMessage(GetDlgItem(hDlg, 1010), BM_GETCHECK, 0, 0));
	REG_WriteLong("DisableSound", SendMessage(GetDlgItem(hDlg, 1018), BM_GETCHECK, 0, 0));
	REG_WriteLong("TextLow", SendMessage(GetDlgItem(hDlg, 1014), BM_GETCHECK, 0, 0));
	REG_WriteLong("BumpLow", SendMessage(GetDlgItem(hDlg, 1015), BM_GETCHECK, 0, 0));
	REG_WriteLong("Window", SendMessage(GetDlgItem(hDlg, 1025), BM_GETCHECK, 0, 0));
	REG_WriteLong("Volumetric", SendMessage(GetDlgItem(hDlg, 1029), BM_GETCHECK, 0, 0));
	REG_WriteLong("NoFMV", SendMessage(GetDlgItem(hDlg, 1030), BM_GETCHECK, 0, 0));
	REG_WriteLong("Setup", 1);

	CloseRegistry();
	return 1;
}

char REG_KeyWasCreated() {
	return dwDisposition == REG_CREATED_NEW_KEY;
}
