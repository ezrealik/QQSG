#pragma once
#include "../QQSG/Module.h"
#include "../QQSG/GameDirectX9.h"
#include "../QQSG/ResourceFile.hpp"
#include "resource.h"
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <mutex>
using namespace ResouceFile;
#pragma pack(push,1)
struct MouseInfo {
	int x;
	int y;
	int KeyCode;
	int KeyState;
	UINT OldTick;
};
#pragma pack(pop)
#pragma region 全局变量
#define ThreadMutexLockDeleay 3500
DirectX92D D2Dx9;
HWND G_hWnd;
BOOL DrawThread = TRUE, ISMove, IsTip;
WNDPROC OldDrawWndProc = nullptr, OldEditWndProc = nullptr, OldEditDelayWndProc = nullptr;
MapImageInfo ImageInfo = { 0 };
MouseInfo MousePoint = { 0 }, Mibfh = { 0 };
HINSTANCE G_hInst;
HANDLE G_lpDrawThread = nullptr;
RECT GlRect, ClRect, lRect;
UINT OldFpsTick = 0, FPSCount = 0, NewFPSCount = 0, SelectIndex = 0x8FFFFFF, SelectAnimateIndex = 0x8FFFFFF, CurrencyIndex = 0, UpIndex = 0x8FFFFFF,
OldKeyTick = 0;
int ImageCreenX = 0, ImageCreenY = 0;
MouseInfo BOMouse;
#pragma endregion
BOOL CALLBACK WndProcc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//D3D绘制线程;
void WINAPI DrawD3D();
//绘制窗口处理函数;
HRESULT CALLBACK DrawWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//图片帧数延时窗口处理函数;
HRESULT CALLBACK EditAnimateDelayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//图片缩放比例窗口处理函数;
HRESULT CALLBACK EditScaleWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//删除当前选中图片;
void WINAPI DeleteImageInfo();
//删除所有图片;
void WINAPI DeleteAllImage();
//将图片置上一层;
void WINAPI LayerUpImage();
//将图片置下一层;
void WINAPI LayerNextImage();
//将图片置顶层;
void WINAPI LayerTopImage();
//将图片置底层;
void WINAPI LayerBottomImage();
//启动绘制线程;
void WINAPI RunDrawD3D();
//关闭绘制线程;
void WINAPI CloseDrawD3D();
//导入背景图片;
void WINAPI ImportImage();
//导入动画图片;
void WINAPI ImportAnimateImage();
//上一帧动画;
void WINAPI UpAnimateFrams();
//下一帧动画;
void WINAPI NextAnimateFrams();
//置顶帧动画;
void WINAPI TopAnimateFrams();
//置底帧动画;
void WINAPI BottomAnimateFrams();
//置上一帧动画;
void WINAPI SetUpAnimateFrams();
//置下一帧动画;
void WINAPI SetNextAnimateFrams();
//隐藏图片图层(不导出图片)
void WINAPI HideImageLayer();
//显示所有隐藏图片图层(导出图片)
void WINAPI ShowAllHideImageLayer();
//导入已有地图文件;
void WINAPI ImportMap();
//显示FPS;
void WINAPI ShowFPS();
//启动线程
void WINAPI CallThreadFunction(void *FuncAddress);