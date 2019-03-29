#pragma once
#ifndef _Module_H
#define _Module_H
#include <Windows.h>
#include <shlobj.h>

void GetExePathW(WCHAR*dirbuf, int len);
void GetExePathA(char*dirbuf, int len);
void CreateDirectoryEc(char* Dir);

#endif