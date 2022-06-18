#include <windows.h>
#define DllExport __declspec (dllexport)

int message()
{
	MessageBox(0, "DLL Injected!", "DLL Injector", MB_OK);
	return 0;
}
BOOL WINAPI  DllMain(
	HANDLE    hinstDLL,
	DWORD     fdwReason,
	LPVOID    lpvReserved)
{
	message();
}
