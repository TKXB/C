#include<Windows.h>
class CILHook{
public:
	CILHook();
	~CILHook();

	BOOL Hook(LPSTR pszModuleName, LPSTR pszFuncName, PROC pfnHookFunc);
	VOID UnHook();
	BOOL ReHook();

private:
	PROC m_pfnOrig;
	BYTE m_bOldBytes[5];
	BYTE m_bNewBytes[5];
};
