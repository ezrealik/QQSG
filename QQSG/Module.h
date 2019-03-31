#pragma once
#ifndef _Module_H
#define _Module_H
#include <Windows.h>
#include <shlobj.h>

#ifdef _UNICODE
#define GetExePath GetExePathW
#define CreateDirectoryEc CreateDirectoryEcW
#else
#define GetExePath GetExePathA
#define CreateDirectoryEc CreateDirectoryEcA
#endif

void GetExePathW(WCHAR*dirbuf, int len);
void GetExePathA(char*dirbuf, int len);
void CreateDirectoryEcA(const char* Dir);
void CreateDirectoryEcW(const WCHAR* Dir);
#endif