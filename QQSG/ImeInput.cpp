#include "ImeInput.h"



ImeInput::ImeInput()
{
}
ImeInput::~ImeInput()
{
}

BOOL ImeInput::GetKeyState() {
	return ImeKeyState;
}
void ImeInput::SetKeyState(BOOL KeySt) {
	ImeKeyState = KeySt;
}
BOOL ImeInput::SetImeChar(const char*Keychar) {
	if (!Keychar)return FALSE;
	if (ImeStr.length() <= MaxLen) {
		memcpy(&ImeKeyChar, Keychar, sizeof(char));
		ImeStr.append(Keychar);
	}
	return TRUE;
}
BOOL ImeInput::SetImeStr(const char*Str) {
	if (!Str)return FALSE;
	if (ImeStr.length() <= MaxLen) {
		ImeStr.append(Str);
	}
	return TRUE;
}
std::string ImeInput::GetImeString() {
	return ImeStr;
}
void ImeInput::SetMaxLen(UINT Max) {
	MaxLen = Max;
}
void ImeInput::Clear() {
	ImeStr = "";
}
void ImeInput::DeleteImeStr() {
	if (ImeStr.length() > 0) {
		if (ImeStr.at(ImeStr.length() - 1) & 0x8000)
			ImeStr.erase(ImeStr.end() - 1);
		ImeStr.erase(ImeStr.end() - 1);
	}
}
void ImeInput::GetIMEString(HWND hWnd)
{
	if (ImeStr.length() >= MaxLen) {
		return;
	}
	HIMC hIMC = ImmGetContext(hWnd);//获取HIMC	
	if (hIMC)
	{
		//这里先说明一下，以输入“中国”为例	
		//切换到中文输入法后，输入“zhongguo”，这个字符串称作IME组成字符串	
		//而在输入法列表中选择的字符串“中国”则称作IME结果字符串	
		static bool flag = false;//输入完成标记：在输入中时，IME组成字符串不为空，置true；输入完成后，IME组成字符串为空，置false	
		DWORD dwSize = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0);	//获取IME组成输入的字符串的长度	
		if (dwSize > 0)//如果IME组成字符串不为空，且没有错误（此时dwSize为负值），则置输入完成标记为true	
		{
			if (flag == false)
			{
				flag = true;
			}
		}
		else if (dwSize == 0 && flag)	//如果IME组成字符串为空，并且标记为true，则获取IME结果字符串	
		{
			int iSize;	//IME结果字符串的大小	
			LPSTR pszMultiByte = NULL;//IME结果字符串指针	
			int ChineseSimpleAcp = 936;//宽字节转换时中文的编码	
			WCHAR *lpWideStr = NULL;//宽字节字符数组	
			dwSize = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);//获取IME结果字符串的大小	
			if (dwSize > 0)	//如果IME结果字符串不为空，且没有错误	
			{
				dwSize += sizeof(WCHAR);//大小要加上NULL结束符	
										//为获取IME结果字符串分配空间	
				if (lpWideStr)
				{
					delete[]lpWideStr;
					lpWideStr = NULL;
				}
				lpWideStr = new WCHAR[dwSize];
				memset(lpWideStr, 0, dwSize);	//清空结果空间	
				ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, lpWideStr, dwSize);//获取IME结果字符串，这里获取的是宽字节	
				iSize = WideCharToMultiByte(ChineseSimpleAcp, 0, lpWideStr, -1, NULL, 0, NULL, NULL);//计算将IME结果字符串转换为ASCII标准字节后的大小	
																									 //为转换分配空间	
				if (pszMultiByte)
				{
					delete[] pszMultiByte;
					pszMultiByte = NULL;
				}
				pszMultiByte = new char[iSize + 1];
				WideCharToMultiByte(ChineseSimpleAcp, 0, lpWideStr, -1, pszMultiByte, iSize, NULL, NULL);//宽字节转换	
				pszMultiByte[iSize] = '\0';
				ImeStr += pszMultiByte;//添加到string中	
				while(ImeStr.length() > MaxLen) {
					if (ImeStr.at(ImeStr.length() - 1) & 0x8000)
						ImeStr.erase(ImeStr.end() - 1);
					ImeStr.erase(ImeStr.end() - 1);
				}
				//释放空间	
				if (lpWideStr)
				{
					delete[]lpWideStr;
					lpWideStr = NULL;
				}
				if (pszMultiByte)
				{
					delete[] pszMultiByte;
					pszMultiByte = NULL;
				}
			}
			flag = false;
		}
		ImmDestroyContext(hIMC);
		ImmReleaseContext(hWnd, hIMC);//释放HIMC	
	}
}
UINT ImeInput::GetImeLen() {
	return ImeStr.length();
}