#pragma once
#include <windows.h>
#include <conio.h>
#include <string>
#include<imm.h>
#pragma comment(lib,"imm32.lib")
class ImeInput
{
public:
	ImeInput();
	~ImeInput();
	//ªÒ»°º¸≈Ã ‰»Î◊¥Ã¨;
	BOOL GetKeyState();
	void SetKeyState(BOOL KeySt);
	BOOL SetImeChar(const char*Keychar);
	BOOL SetImeStr(const char*Str);
	std::string GetImeString();
	void SetMaxLen(UINT Max);
	void DeleteImeStr();
	void Clear();
	UINT GetImeLen();
	void GetIMEString(HWND hWnd);
private:
	BOOL ImeKeyState = FALSE;
	char ImeKeyChar;
	UINT MaxLen;
	std::string ImeStr;
};

