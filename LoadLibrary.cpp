#include <Windows.h>
#include <stdio.h>
typedef int (__cdecl *MYPROC)(int,int); //函数指针: int 表示函数的返回值为 int
										//(__cdecl *MYPROC) 说明这是一个函数指针，调用方式为 __cdecl
										//(int,int)说明此函数有2个参数，类型为 int 
int main(){
	char szCurrentPath[MAX_PATH] = { 0 };
	strcat(szCurrentPath, "D:\\C++source\\Program module\\Dll\\DLLGenerator.dll");
	HINSTANCE hMod = LoadLibrary(szCurrentPath);
	MYPROC add = (MYPROC)GetProcAddress(hMod, "Add");
	int result = add(2,3);
	printf("result is %d", result);
	FreeLibrary(hMod);
	system("pause");
	return 0;
}
