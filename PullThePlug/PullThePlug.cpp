#define _AFXDLL
#include <afx.h>
#include <iostream>
#include <windows.h>
#include <winternl.h>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	typedef ULONG32(WINAPI *lpNtShutdownSystem)(int Action);
	typedef ULONG32(WINAPI *lpNtSetSystemPowerState)(IN POWER_ACTION SystemAction, IN SYSTEM_POWER_STATE MinSystemState, IN ULONG32 Flags);
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	PVOID Info;
	HMODULE hModule;
	lpNtSetSystemPowerState NtSetSystemPowerState;
	lpNtShutdownSystem NtShutdownSystem;

	//Load ntdll.dll
	if ((hModule = LoadLibrary(_T("ntdll.dll"))) == 0) {
		std::cerr << "Error opening ntdll.dll" << std::endl;
		return 1;  
	}

	//Get functions
	NtShutdownSystem = (lpNtShutdownSystem)GetProcAddress(hModule, "NtShutdownSystem");
	if (NtShutdownSystem == NULL) {
		std::cerr << "Error getting NtShutdownSystem" << std::endl;
		return 2;
	}
	NtSetSystemPowerState = (lpNtSetSystemPowerState)GetProcAddress(hModule, "NtSetSystemPowerState");
	if (NtSetSystemPowerState == NULL) {
		std::cerr << "Error getting NtSetSystemPowerState" << std::endl;
		return 3;
	}

	// Get a token for this process
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return(FALSE);

	// Get the LUID for the shutdown privilege
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;  // one privilege to set	
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process. 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS) {
		std::cerr << "Error getting privilege" << std::endl;
		return 4;
	}
	
	/*
	* Technically only NtSetSystemPowerState is needed to be called to power off a computer
	* Howver, I found at least one report of NtSetSystemPowerState not working while NtShutdownSystem does
	* https://www.autoitscript.com/forum/topic/149641-how-to-force-a-power-down/page/2/?tab=comments#comment-1166299
	* So the code calls NtSetSystemPowerState first, since in my tests it's a hair faster, and if that fails will call NtShutdownSystem as a fallback
	*/
	ULONG32 retNSSPS = NtSetSystemPowerState((POWER_ACTION)PowerSystemShutdown, (SYSTEM_POWER_STATE)PowerActionShutdown, 0);
	ULONG32 retNSS = NtShutdownSystem(2); //2 = ShutdownPowerOff

	//At this point the PC is shut down if no problems have occurred
	std::cerr << "System didn't shutdown" << std::endl << "NtSetSystemPowerState returned " << retNSSPS << std::endl << "NtShutdownSystem returned " << retNSS << std::endl;
	return 5;
}