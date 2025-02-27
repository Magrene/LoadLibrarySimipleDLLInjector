// DLLinjectorFirst.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

#define WIN32_LEAN_AND_MEAN
#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ) 

BOOL Inject(DWORD pid, const char* DLL_LOCATION);
BOOL WindowsProcMy(HWND hwnd, LPARAM lParam);
HWND g_HWND = NULL;
int main()
{
	string windowName;
	BOOL pidSearch;
	string dllstr;
	while (windowName != "exit" && dllstr != "exit") {
		
		cout << "Enter the window name of the process to inject into" << endl;
		cin >> windowName;



		//HWND hwnd = FindWindow(0, windowNamefunc);
		HWND hwnd = FindWindowA(0, windowName.c_str());

		//HWND hwnd = GetWindowThreadProcessId(hwnd, procID);
		if (hwnd == 0) {
			cout << "Error could not find program" << endl;
			cin.get();
		}
		else {
			cout << "Program found" << endl;
			DWORD pid;
			GetWindowThreadProcessId(hwnd, &pid);
			cout << pid << endl;
			cout << "Please enter a dll filename with extension to inject" << endl;
			
			cin >> dllstr;
			const char* DLL_NAME = dllstr.c_str();
			// Get the dll's full path name 
			char DLL_LOCATION[MAX_PATH] = { 0 };
			GetFullPathName(DLL_NAME, MAX_PATH, DLL_LOCATION, NULL);

			//Call inject function
			if (!Inject(pid, DLL_LOCATION)) {
				cout << "Failed to inject" << endl;
			}
			else {
				cout << "Injection succeeded" << endl;
			}
		}
		
	}
	return 0;
}

BOOL Inject(DWORD pid, const char *DLL_LOCATION) {
	HMODULE hLib;
	char buf[50] = { 0 };
	LPVOID RemoteString, LoadLibAdd;

	if (!pid) {
		return false;
	}

	// open process thread 
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProc) {
		sprintf_s(buf, "OpenProcess failed: %d", GetLastError());
		printf(buf);
		return false;
	}

	// Load Library called
	LoadLibAdd = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

	// Allocate space in the process for our DLL
	RemoteString = (LPVOID)VirtualAllocEx(hProc, NULL, strlen(DLL_LOCATION), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	// Write the string name of dll into memory
	if (WriteProcessMemory(hProc, (LPVOID)RemoteString, DLL_LOCATION, strlen(DLL_LOCATION), NULL) == false) {return false;}
		
	CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAdd, (LPVOID)RemoteString, NULL, 0);

	CloseHandle(hProc);
	
	return true;
}


BOOL WindowsProcMy(HWND hwnd, LPARAM lParam)
{
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (lpdwProcessId == lParam)
	{
		g_HWND = hwnd;
		return FALSE;
	}
	return TRUE;
}
