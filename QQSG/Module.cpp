#include "Module.h"

C_Module::C_Module() {
	ZeroMemory(szExePathA, sizeof(szExePathA));
	ZeroMemory(szExePathW, sizeof(szExePathW));
}
C_Module::~C_Module(){

}
//取运行目录(UNICODE编码)
const wchar_t *C_Module::GetExePathW() {
	GetModuleFileNameW(NULL, szExePathW, sizeof(szExePathW));
	(wcsrchr(szExePathW, '\\'))[1] = 0;
	return szExePathW;
}
//取运行目录(ANSII编码);
const char *C_Module::GetExePathA() {
	GetModuleFileNameA(NULL, szExePathA, sizeof(szExePathA));
	(strrchr(szExePathA, '\\'))[1] = 0;
	return szExePathA;
}
//创建多级目录;
int C_Module::CreateDirectoryEcA(const char *Dir) const{
	char NewDir[256] = { 0 };
	CopyMemory(NewDir, Dir, strlen(Dir) * sizeof(char));
	(strrchr(NewDir, '\\'))[1] = 0;
	return SHCreateDirectoryExA(nullptr, (LPCSTR)NewDir, nullptr);
}
//创建多级目录;
int C_Module::CreateDirectoryEcW(const WCHAR *Dir) const{
	WCHAR NewDir[256] = { 0 };
	CopyMemory(NewDir, Dir, wcslen(Dir) * sizeof(WCHAR));
	(wcsrchr(NewDir, '\\'))[1] = 0;
	return SHCreateDirectoryExW(nullptr, NewDir, nullptr);
}
const wchar_t *C_Module::GetCurrencyPathFileW(const WCHAR* Dir) {
	GetModuleFileNameW(NULL, szCurrencyPathW, sizeof(szCurrencyPathW));
	(wcsrchr(szCurrencyPathW, '\\'))[1] = 0;
	wcscat(szCurrencyPathW, Dir);
	return szCurrencyPathW;
}
const char *C_Module::GetCurrencyPathFileA(const char* Dir) {
	GetModuleFileNameA(NULL, szCurrencyPathA, sizeof(szCurrencyPathA));
	(strrchr(szCurrencyPathA, '\\'))[1] = 0;
	strcat(szCurrencyPathA, Dir);
	return szCurrencyPathA;
}
void MsgTipExit(const char *pTip){
	MessageBoxA(NULL, pTip, "Error:", MB_OK | MB_ICONERROR);
	ExitProcess(NULL);
}
