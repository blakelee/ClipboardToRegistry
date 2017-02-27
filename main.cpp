#include <string>
#include <objbase.h>
#using <System.dll>
#include <tlhelp32.h>
#define WINAPI __stdcall

using namespace System;
using namespace std;
using namespace Microsoft::Win32;

DWORD FindProcessId(const wstring& processName);
int DisplayResourceNAMessageBox(string newlast);
void openRegistry();
char* getClipboard();
void setKey(string newlast);

int WINAPI WinMain(__in HINSTANCE hInstance,
	__in_opt HINSTANCE hPrevInstance,
	__in LPSTR lpCmdLine,
	__in int nShowCmd)
{

	string str[] = {
		"HKCR", "HKEY_CLASSES_ROOT",	 // 0 ... 1
		"HKCU", "HKEY_CURRENT_USER",	 // 2 ... 3
		"HKLM", "HKEY_LOCAL_MACHINE",    // 4 ... 5
		"HKU", "HKEY_USERS",			 // 6 ... 7
		"HKCC", "HKEY_CURRENT_CONFIG" }; // 8 ... 9

	string newlast = getClipboard();
	bool match = false;
	int i = 0;

	string subProcessName = "regedit.exe";

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &subProcessName[0], (int)subProcessName.size(), NULL, 0);
	wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &subProcessName[0], (int)subProcessName.size(), &wstrTo[0], size_needed);

	DWORD processID = FindProcessId(wstrTo);

	HANDLE h = OpenProcess(PROCESS_TERMINATE, false, processID);
	TerminateProcess(h, 1);
	CloseHandle(h);

	while (i < 10 && match == false)
	if (str[i].compare(0, str[i].size(), newlast, 0, str[i].size()) == 0)
	{
		if (i == 0 || i == 2 || i == 4 || i == 6 || i == 8)
		{
			size_t pos = 0;
			while ((pos = newlast.find(str[i], pos)) != string::npos) {
				newlast.replace(pos, str[i].length(), str[i + 1]);
				pos += str[i + 1].length();
			}
		}
		match = true;
		setKey(newlast);
		openRegistry();
	}
	else
	{
		i++;
		if (i >= 10 && sizeof(newlast) != NULL)
			DisplayResourceNAMessageBox(newlast);
	}
	exit(0);
}

int DisplayResourceNAMessageBox(string newlast)
{
	if (newlast.front() == 13)
	{
		string notfound = "No text in clipboard";
		wstring stemp = wstring(notfound.begin(), notfound.end());
		LPCWSTR sw = stemp.c_str();

		int msgboxID = MessageBox(
			NULL,
			(LPCWSTR)sw,
			(LPCWSTR)L"Error!",
			MB_ICONEXCLAMATION
			);
		return msgboxID;
	}
	else 
	{
		string notfound = "Must start with HKCR, HKCU, HKLM, HKU, or HKCC.\n";
		notfound += "Their lengthened counterparts also work.\n\nThe text in your clipboard was:\n\n";
		notfound += newlast;
 
		wstring stemp = wstring(notfound.begin(), notfound.end());
		LPCWSTR sw = stemp.c_str();

		int msgboxID = MessageBox(
			NULL,
			(LPCWSTR)sw,
			(LPCWSTR)L"Text not identified",
			MB_ICONEXCLAMATION
			);
 
		return msgboxID;
	}
}


void setKey(string newlast)
{
	String^ lastkey = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Regedit\\";
	String^ lastkey_key = "LastKey";
	String^ SysNewlast = gcnew String(newlast.c_str());
	Registry::SetValue(lastkey, lastkey_key, SysNewlast);
}

Object^ readKey(String^ valuename, String^ value)
{
	String^ keyname = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\ClipboardToRegistry.exe"; //default location I will modify
	Object^ regval = Registry::GetValue(keyname, valuename, value);
	if (regval != nullptr)
		return regval;
	return nullptr;
}

char* getClipboard()
{
	HANDLE clip = NULL;
	OpenClipboard(NULL);
	clip = GetClipboardData(CF_TEXT);
	if (clip != NULL)
	return (char*)clip;
	else return "";
}
void openRegistry()
{
	string s = "regedit.exe";
	wstring stemp = wstring(s.begin(), s.end());
	LPCWSTR sw = stemp.c_str();
	ShellExecute(NULL, NULL, sw, NULL, NULL, SW_SHOWNORMAL);
}

DWORD FindProcessId(const wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}
	return 0;
}