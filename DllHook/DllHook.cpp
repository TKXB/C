#include "ILHook.h"

CILHook CreateProcessHook;
HANDLE WINAPI MyCreateFileW(
	LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
	)
{

	BOOL bRet = FALSE;
	HANDLE handle;
	CreateProcessHook.UnHook();
	MessageBoxW(NULL, lpFileName, lpFileName, MB_OK);
	handle = CreateFileW(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile
		);
	CreateProcessHook.ReHook();
	if (handle != NULL){
		return handle;
	}
	else{
		return FALSE;
	}
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call){
	case DLL_PROCESS_ATTACH:
		{
			MessageBox(NULL,"HELLO","TEST",MB_OK);
			CreateProcessHook.Hook("kernel32.dll","CreateFileW",(PROC)MyCreateFileW);
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			CreateProcessHook.UnHook();
			break;
		}
	}

	return TRUE;
}




