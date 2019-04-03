#pragma once
#ifndef _GameDirectX9_H
#define _GameDirectX9_H
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include "Module.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")
#pragma warning(disable:4996)

class DirectX92D
{
public:
	//函数定义;
	DirectX92D();
	~DirectX92D();
	//初始化D3Directx9;
	BOOL InitD3D(HWND hWnd, int Width, int Height, bool FullScreen);
	//获取图片信息;
	D3DXIMAGE_INFO GetImageInfo(const wchar_t *filename);
	//获取内存图片信息;
	D3DXIMAGE_INFO GetImageInfoInMemory(void *SrcData, UINT SrcLen);
	//载入内存纹理;
	LPDIRECT3DTEXTURE9 LoadMemTexture(void* ImgStream, UINT Imglen, D3DCOLOR transcolor = D3DCOLOR_XRGB(0, 0, 0));
	//载入纹理文件;
	LPDIRECT3DTEXTURE9 LoadTexture(const wchar_t* filename, D3DCOLOR transcolor = D3DCOLOR_XRGB(0, 0, 0));
	//载入纹理文件;
	LPDIRECT3DTEXTURE9 LoadRectTexture(const wchar_t* filename, UINT Width, UINT Height, D3DCOLOR transcolor = D3DCOLOR_XRGB(0, 0, 0));
	//绘制一个材质;(材质,x,y,宽度,高度,缩放比例,旋转方向,颜色);
	BOOL DrawTexture(LPDIRECT3DTEXTURE9 texture, float x, float y, UINT width = 50, UINT height = 50, float scale = 1.0f, float rotation = 0.0f, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));
	// 绘制逐帧动画;(材质,x,y,宽度,高度,绘制帧,图片最大帧数,缩放比例,旋转方向,颜色);
	BOOL DrawAnimateTexture(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, int frame = 0, int columns = 1, float scale = 1.0f, float rotation = 0.0f, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));
	//绘制图片中心点材质;;(图片文件,x,y,宽度,高度,缩放比例,旋转方向,颜色);
	BOOL DrawImageCenterTexture(const wchar_t *ImageName, int x, int y, int width = 50, int height = 50, float scale = 1.0f, float rotation = 0.0f, D3DCOLOR Imagecolor = D3DCOLOR_XRGB(0, 0, 0), D3DCOLOR Drawcolor = D3DCOLOR_XRGB(255, 255, 255));
	//绘制图片自定义大小材质;;(图片文件,x,y,宽度,高度,缩放比例,旋转方向,颜色);
	BOOL DrawImageRectTexture(const wchar_t *ImageName, int x, int y, int width, int height, float scale = 1.0f, float rotation = 0.0f, D3DCOLOR Imagecolor = D3DCOLOR_XRGB(0, 0, 0), D3DCOLOR Drawcolor = D3DCOLOR_XRGB(255, 255, 255));
	//绘制图片材质;;(图片文件,x,y,宽度,高度,缩放比例,旋转方向,颜色);
	BOOL DrawImageTexture(const wchar_t *ImageName, int x, int y, int width = 50, int height = 50, float scale = 1.0f, float rotation = 0.0f, D3DCOLOR Imagecolor = D3DCOLOR_XRGB(0, 0, 0), D3DCOLOR Drawcolor = D3DCOLOR_XRGB(255, 255, 255));
	//绘制直线;
	BOOL DrawLineto(float x1, float y1, float x2, float y2, float Width = 1.0f, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));
	//绘制文字;
	BOOL DrawFont(const char *OutFont, int FontSize, int Weight, BOOL ltalic, const char *FontName, LPRECT rect, D3DCOLOR Color = D3DCOLOR_XRGB(0, 0, 0));
	//绘制文本字体;
	BOOL DrawTextFont(const char *OutFont, int FontSize, int Weight, BOOL ltalic, const char *FontName, LPRECT rect, D3DCOLOR Color = D3DCOLOR_XRGB(0, 0, 0));
	//获取D3Dx9对象
	LPDIRECT3D9 GetDirectx9();
	//获取D3D设备对象;
	LPDIRECT3DDEVICE9 GetD3Devicex9();
	//获取缓冲区对象;
	LPDIRECT3DSURFACE9 GetD3DSurfacex9();
	HRESULT Clear(DWORD Count, D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
	//绘制矩形;
	void DrawRectagle(float left, float Right, float up, float bottom, float Width = 1.0f, D3DCOLOR Color = D3DCOLOR_XRGB(0, 0, 0));

private:
	//定义D3D全局变量;
	LPDIRECT3D9 Directx9;
	LPDIRECT3DDEVICE9 D3DDevice9;
	LPDIRECT3DSURFACE9 D3DSurface9;
	LPD3DXLINE Dxline;
	LPD3DXFONT DxFont;
	LPD3DXSPRITE D3DSprite;
	D3DXIMAGE_INFO Imginfo = { 0 };
};
#endif