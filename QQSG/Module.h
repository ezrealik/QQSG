#pragma once
#ifndef _Module_H
#define _Module_H
#include <Windows.h>
#include <shlobj.h>
#include <math.h>
#include "Bmpdata.h"
#pragma warning(disable:4996)
using namespace std;
typedef struct tag_color_32 {
	BYTE Red;
	BYTE Green;
	BYTE Blue;
	BYTE Alpha;
}color_32,*PColor_32;
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
	BOOL MirrorBitmap(void *pImageAlloc, UINT pLen, BOOL Vertical = FALSE);
private:
	WCHAR szExePathW[MAX_PATH];
	char szExePathA[MAX_PATH];
	WCHAR szCurrencyPathW[MAX_PATH];
	char szCurrencyPathA[MAX_PATH];
};
void MsgTipExit(const char *pTip);
#endif