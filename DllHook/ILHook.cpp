#include"ILHook.h"
CILHook::CILHook()
{
	m_pfnOrig = NULL;
	ZeroMemory(m_bOldBytes, 5);
	ZeroMemory(m_bNewBytes, 5);
}

CILHook::~CILHook(){
	UnHook();

	m_pfnOrig = NULL;
	ZeroMemory(m_bOldBytes, 5);
	ZeroMemory(m_bNewBytes, 5);
}

BOOL CILHook::Hook(LPSTR pszModuleName, LPSTR pszFuncName, PROC pfnHookFunc){
	BOOL bRet = FALSE;
	m_pfnOrig = (PROC)GetProcAddress(GetModuleHandle(pszModuleName), pszFuncName);

	if( m_pfnOrig != NULL){
		DWORD dwNum = 0;
		ReadProcessMemory(GetCurrentProcess(), m_pfnOrig, m_bOldBytes, 5, &dwNum);
		m_bNewBytes[0] = '\xe9';      //jmp opcode
		*(DWORD *)(m_bNewBytes + 1) = (DWORD)pfnHookFunc - (DWORD)m_pfnOrig - 5;    //jmp后的偏移量 = 目标地址 - 原地址 - 5 （5：jump 0xabcdefgh）
		WriteProcessMemory(GetCurrentProcess(), m_pfnOrig, m_bNewBytes, 5, &dwNum);
		bRet = TRUE;
	}
	return bRet;
}

VOID CILHook::UnHook(){
	if( m_pfnOrig !=0 ){
		DWORD dwNum = 0;
		WriteProcessMemory(GetCurrentProcess(), m_pfnOrig, m_bOldBytes, 5, &dwNum);
	}
}

BOOL CILHook::ReHook(){
	BOOL bRet = FALSE;

	if (m_pfnOrig != 0){
		DWORD dwNum = 0;
		WriteProcessMemory(GetCurrentProcess(), m_pfnOrig, m_bNewBytes, 5, &dwNum);
		bRet = TRUE;
	}
	return bRet;
}
