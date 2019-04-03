#pragma once
#ifndef _Module_H
#define _Module_H
#include <Windows.h>
#include <shlobj.h>
#pragma warning(disable:4996)
using namespace std;
class C_Module {
public:
	C_Module();
	~C_Module();
	const wchar_t *GetExePathW();
	const char *GetExePathA();
	const wchar_t *GetCurrencyPathFileW(const WCHAR *FileName);
	const char *GetCurrencyPathFileA(const char *FileName);
	int CreateDirectoryEcA(const char* Dir) const;
	int CreateDirectoryEcW(const WCHAR* Dir) const;
private:
	WCHAR szExePathW[MAX_PATH];
	char szExePathA[MAX_PATH];
	WCHAR szCurrencyPathW[MAX_PATH];
	char szCurrencyPathA[MAX_PATH];
};
void MsgTipExit(const char *pTip);

#endif