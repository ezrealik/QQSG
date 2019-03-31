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
BOOL CALLBACK WndProcc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//D3D绘制线程;
void WINAPI DrawD3D();
//绘制窗口处理函数;
HRESULT CALLBACK DrawWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
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
//启动线程
void WINAPI CallThreadFunction(void *FuncAddress);