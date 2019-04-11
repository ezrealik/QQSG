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
//位图镜像
BOOL C_Module::MirrorBitmap(void *pImageAlloc, UINT pLen, BOOL Vertical) {
	BITMAPFILEHEADER BitmapHeader;
	BITMAPINFOHEADER BitmapInfo;
	PColor_32 pTmp = nullptr;
	//读取位图都文件;
	if (pLen < sizeof(BitmapHeader))return FALSE;
	CopyMemory(&BitmapHeader, pImageAlloc, sizeof(BitmapHeader));
	//读取位图信息;
	if (pLen < BitmapHeader.bfSize)return FALSE;
	pTmp = (PColor_32)((char*)pImageAlloc + sizeof(BitmapHeader));
	CopyMemory(&BitmapInfo, pTmp, sizeof(BitmapInfo));
	//移动指针偏移至位图颜色数据区;
	pTmp = (PColor_32)((char*)pImageAlloc + BitmapHeader.bfOffBits);
	//是否32位alpha通道位图;
	if (BitmapInfo.biBitCount == 32) {
		//是否垂直镜像;
		if (!Vertical) {
			//水平镜像;
			UINT MapHeight = 0, MapWidth = 0;
			//取位图绝对值(因为有的位图是负数反向图像)
			//我们直接镜像,因为即便反向图像,我们也是反向镜像所以不影响实际结果;
			MapHeight = abs(BitmapInfo.biHeight);
			MapWidth = BitmapInfo.biWidth;
			//计算位图数据大小,也可以取biSizeImage;
			UINT RGBLen = MapHeight * MapWidth * sizeof(color_32);
			//申请新位图存储的内存空间;
			PColor_32 BitmapRGB = (PColor_32)LocalAlloc(LMEM_ZEROINIT, RGBLen);
			if (!BitmapRGB)return FALSE;
			//每行位图数据;
			for (UINT i = 0; i < MapHeight; i++) {
				//每列位图数据;(因为32位图数据是4字节存储,因此无需考虑对齐问题);
				for (UINT n = 0; n < MapWidth; n++) {
					//位图数据镜像置换;
					UINT index = i * MapWidth + n, NextIndex = i * MapWidth + MapWidth - 1 - n;
					BitmapRGB[index] = pTmp[NextIndex];
				}
			}
			CopyMemory(pTmp, BitmapRGB, RGBLen);
			LocalFree(BitmapRGB);
		}
		else {
			//垂直镜像;
			UINT MapHeight = 0, MapWidth = 0;
			//取位图绝对值(因为有的位图是负数反向图像)
			//我们直接镜像,因为即便反向图像,我们也是反向镜像所以不影响实际结果;
			MapHeight = abs(BitmapInfo.biHeight);
			MapWidth = BitmapInfo.biWidth;
			//计算位图数据大小,也可以取biSizeImage;
			UINT RGBLen = MapHeight * MapWidth * sizeof(color_32);
			//申请新位图存储的内存空间;
			PColor_32 BitmapRGB = (PColor_32)LocalAlloc(LMEM_ZEROINIT, RGBLen);
			if (!BitmapRGB)return FALSE;
			//每行位图数据;
			for (UINT i = 0; i < MapHeight; i++) {
				//每列位图数据;(因为32位图数据是4字节存储,因此无需考虑对齐问题);
				for (UINT n = 0; n < MapWidth; n++) {
					//位图数据镜像置换;
					UINT index = i * MapWidth + n, NextIndex = (MapHeight - 1 - i)*MapWidth + n;
					BitmapRGB[index] = pTmp[NextIndex];
				}
			}
			CopyMemory(pTmp, BitmapRGB, RGBLen);
			LocalFree(BitmapRGB);
		}
	}
	return TRUE;
}
