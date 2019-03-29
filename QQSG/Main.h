#pragma once
#include "bass.h"
#include "Module.h"
#include "GameDirectX9.h"
#include "zlib\zlib.h"
#include "ResourceFile.h"
#include "ImeInput.h"
#include <windows.h>
#include <windowsx.h>


#pragma comment(lib,"bass.lib")
#pragma warning(disable:4996)

#define CL_WinWidth 1024
#define CL_WinHeight 768
#pragma pack(push,1)
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
typedef struct _ImageTexturInfo
{
	int MaxImage;
	int x;
	int y;
	int width;
	int height;
	int MaskLayer;
	float Scale;
	float rotation;
	LPDIRECT3DTEXTURE9 Texture;
	const wchar_t *ImageFile;
}ImageTexturInfo, *PImageTexturInfo;
typedef struct _NPCImageInfo
{
	PImageTexturInfo TextureInfo;
	int iflag;
	int CurrencyImage;
	int OldTickCount;
}NPCImageInfo, *PNPCImageInfo;
#pragma pack(pop)

//窗口处理函数;
LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//游戏渲染函数;
void D3DGameRun();
//播放登录背景音乐;
void PlayerBGM_Login(const char *MusicName);
//绘制玩家;
void DrawPlayerCarrorYY(PNPCImageInfo _PlayerInfo, int PlayDelay);
//绘制玩家角色创建;
void DrawCreatePlayer();
//输入法处理函数;
void ImeEvent(WPARAM wParam);
//绘制提示框;
void DrawTipBox();
