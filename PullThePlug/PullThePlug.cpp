#define _AFXDLL
#include <afx.h>
#include <iostream>
#include <windows.h>
#include <winternl.h>


int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    typedef ULONG32(WINAPI *lpNSSP)(IN POWER_ACTION SystemAction, IN SYSTEM_POWER_STATE MinSystemState, IN ULONG32 Flags);
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    PVOID Info;
    HMODULE hModule;
    lpNSSP lp_NSSP;

    //Load ntdll.dll
    if ((hModule = LoadLibrary(_T("ntdll.dll"))) == 0) {
        std::cerr << "Error opening ntdll.dll" << std::endl;
        return 1;
    }

    //Get function
    lp_NSSP = (lpNSSP)GetProcAddress(hModule, "NtSetSystemPowerState");
    if (lp_NSSP == NULL) {
        std::cerr << "Error getting NtSetSystemPowerState" << std::endl;
        return 2;
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
        return 2;
    }

    ULONG32 ret = lp_NSSP((POWER_ACTION)PowerSystemShutdown, (SYSTEM_POWER_STATE)PowerActionShutdown, 0);
    //At this point the PC is shut down if no problems have occurred
    
    std::cerr << "System didn't shutdown" << std::endl << "NtSetSystemPowerState returned " << ret << std::endl;
    return 3;
}