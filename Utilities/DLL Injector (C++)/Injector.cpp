//Author: Tyler Price

//DLL Injector C++

//Enumerates Processes and Injects a DLL Into User Defined Process

//Test.dll is Provided

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

using namespace std;

void PrintProcessNameAndID(DWORD processID)

{	//Enumerating Unknown Processes

	TCHAR szProcessName[MAX_PATH] = TEXT("<invalid>");

	// Get a handle to the process

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |PROCESS_VM_READ, FALSE, processID);

	// Get the process name

	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
		}
	}

	// Print the process name and identifier

	_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);

}


int main(int argc, char *argv[])
{
	//Defining DLL Path, and Process ID

	char *buffer = new char[100];

	int procID;

	int file;

	// Get the list of process identifiers

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return 1;
	}


	// Calculate how many process identifiers were returned

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process

	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			PrintProcessNameAndID(aProcesses[i]);
		}
	}

	//Specify PID and DLL Path

	cout << "[!] Enter PID: ";

	cin >> procID;

	cout << "[!] Enter DLL Path: ";

	cin >> buffer;

	//Check if DLL Exists 

	file = PathFileExists(buffer);

	if (file == 1)
	{
		printf("[*] File Found!\n");
	}

	else
	{
		printf("[!] ERROR: Enter Vaild DLL Path...\n");

		system("pause");

		exit(EXIT_FAILURE);

	}

	//Opening Process

	printf("[*] Opening Process...\n");

	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

	if (process == NULL)
	{
		printf("[!] Process not found...\n");
	}

	//Loading LoadLibraryA() Function

	printf("[*] Loading LoadLibraryA()...\n");

	LPVOID addr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

	if (addr == NULL)
	{
		printf("[!] ERROR: LoadLibraryA() Function Not Found...\n");
	}

	//Allocating Memory for DLL 

	printf("[*] Allocating Memory...\n");

	LPVOID arg = (LPVOID)VirtualAllocEx(process, NULL, strlen(buffer), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (arg == NULL)
	{
		printf("[!] ERROR: Memmory Allocation Failed...\n");
	}

	//Write To Process Memory

	printf("[*] Writing Process...\n");

	int n = WriteProcessMemory(process, arg, buffer, strlen(buffer), NULL);

	if (n == 0)
	{
		printf("[!] ERROR: No bytes written to address space...\n");

	}
	
	//Excuting Remote Thread

	printf("[*] Creating Remote Thread...\n");

	HANDLE threadID = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)addr, arg, NULL, NULL);

	if (threadID == NULL)
	{
		printf("[!] ERROR: Could not Create Remote Thread...\n");
	}

	else
	{
		printf("[*] Injection Successful...\n");
	}

	CloseHandle(process);

	system("pause");

	return 0;

}
