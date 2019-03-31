#pragma once
#include "bass.h"
#include "Module.h"
#include "GameDirectX9.h"
#include "zlib\zlib.h"
#include "ResourceFile.hpp"
#include "ImeInput.h"
#include <windows.h>
#include <windowsx.h>

#pragma comment(lib,"bass.lib")
#pragma warning(disable:4996)

#define CL_WinWidth 1024
#define CL_WinHeight 768
#pragma pack(push,1)
#pragma region 全局数据结构
struct MouseInfo {
	int x;
	int y;
	int KeyCode;
};
enum GameMaskLayer {
	Top,
	First,
	Second,
	Third,
	Four,
	Bottom,
};
typedef struct _DrawMapInfo
{
	float x;
	float y;
	UINT width;
	UINT height;
	float Scale;
	float rotation;
	UINT ImgLoadType;
	LPDIRECT3DTEXTURE9 Texture;
	void *ResAlloc;
	_DrawMapInfo *Animate;
	UINT AnimateMaxCount;
	UINT AnimateTickIndex;
	UINT AnimateOldTick;
	UINT AnimateDelay;
}DrawMapInfo, *PDrawMapInfo;
typedef struct _DrawImageInfo
{
	PDrawMapInfo DrawMap;
	UINT MaxInt;
}DrawImageInfo, *PDrawImageInfo;
#pragma pack(pop)
#pragma endregion

#pragma region //全局变量
const WCHAR ClassName[] = L"QQSGDiy";
WCHAR ExePath[256] = { 0 };
HWND G_hWnd;
BOOL DrawGame = TRUE, DrawTip = FALSE, DrawCreatPlyer = TRUE, IsMan = FALSE;
void *BGM_Login1 = nullptr;
HSTREAM StreamBGM1;
DirectX92D D2Dx9;
ImeInput Ime;
MouseInfo MousePoint = { 0 };
UINT Countf = 0, OldFpsTick = 0, FPSCount = 0, NewFPSCount = 0, OldTickCount = 0, PlayerIndex = 0, Country = 0;
DrawImageInfo DrawImgInf = { 0 };
#pragma endregion
//窗口处理函数;
LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//游戏渲染函数;
void D3DGameRun();
//播放登录背景音乐;
void PlayerBGM_Login(const char *MusicName);
//显示FPS帧率:
void ShowFPS();
//初始化绘制创建玩家资源;
void InitDrawCreatePlayer();
//绘制玩家角色创建;
void DrawCreatePlayer();
//输入法处理函数;
void ImeEvent(WPARAM wParam);
//绘制提示框;
void DrawTipBox();
