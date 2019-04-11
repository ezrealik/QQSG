#pragma once
//---------------------------------------------------------------------------
#ifndef	BmpDataH
#define	BmpDataH

#include <windows.h>
#include <algorithm>
using std::min;
using std::max;
#include <gdiplus.h>

#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;
//---------------------------------------------------------------------------

#define	ScanAllocFlag		0x00000100
#define PixelAlphaFlag  	0x00010000
//---------------------------------------------------------------------------

// 定义象素插值方式
typedef enum
{
	InterpolateModeDefault,	// 缺省为线形插值
	InterpolateModeNear,	// 临近插值
	InterpolateModeBilinear,// 线形插值
	InterpolateModeBicubic	// 双立方插值
}InterpolateMode;

typedef enum
{
	//	PixelFormatInValid,
	PixelFormat1bit,
	PixelFormat4bit,
	PixelFormat8bit,
	PixelFormat15bit,
	PixelFormat16bit,
	PixelFormat24bit,
	PixelFormat32bit
}ImagePixelFormat;

// 定义ARGB像素结构
typedef union
{
	ARGB Color;
	struct
	{
		BYTE Blue;
		BYTE Green;
		BYTE Red;
		BYTE Alpha;
	};
}ARGBQuad, *PARGBQuad;
//---------------------------------------------------------------------------

FORCEINLINE
VOID SetAlphaFlag(BitmapData *data, BOOL isAlpha)
{

	if (isAlpha) data->Reserved |= PixelAlphaFlag;

	else data->Reserved &= ~PixelAlphaFlag;

}

//---------------------------------------------------------------------------

FORCEINLINE
BOOL HasAlphaFlag(CONST BitmapData *data)
{
	return (data->Reserved & PixelAlphaFlag) != 0;
}
//---------------------------------------------------------------------------

FORCEINLINE
VOID SetInterpolateMode(BitmapData *data, InterpolateMode mode)
{
	data->Reserved = (data->Reserved & 0xffffff) | (mode << 24);
}
//---------------------------------------------------------------------------

FORCEINLINE
InterpolateMode GetInterpolateMode(CONST BitmapData *data)
{

	return (InterpolateMode)(data->Reserved >> 24);

}

//---------------------------------------------------------------------------


// 锁定GDI+32位位图扫描线到data
FORCEINLINE
VOID LockBitmap(Gdiplus::Bitmap *bmp, BitmapData *data)
{
	Gdiplus::Rect r(0, 0, bmp->GetWidth(), bmp->GetHeight());
	BOOL hasAlpha = bmp->GetPixelFormat() & PixelFormatAlpha;
	bmp->LockBits(&r, ImageLockModeRead | ImageLockModeWrite,
		PixelFormat32bppARGB, data);
	SetAlphaFlag(data, hasAlpha);
}
//---------------------------------------------------------------------------

// GDI+位图扫描线解锁
FORCEINLINE
VOID UnlockBitmap(Gdiplus::Bitmap *bmp, BitmapData *data)
{
	data->Reserved &= 0xff;
	bmp->UnlockBits(data);
}
//---------------------------------------------------------------------------

// 用给定的图像数据制造并返回与GDI+兼容的32位位图数据结构。
// 参数；宽度，高度，扫描线宽度，扫描线首地址，alpha标记，返回的位图数据结构指针。
// 注：如果stride=0，自动计算扫描线宽度
FORCEINLINE
VOID GetBitmapData(INT width, INT height, INT stride, LPVOID scan0,
	ImagePixelFormat format, BOOL isAlpha, BitmapData *data)
{
	INT bits[] = { 0x100, 0x400, 0x800, 0x1005, 0x1000, 0x1800, 0x2000 };

	data->Width = width;
	data->Height = height;
	data->Scan0 = scan0;
	data->PixelFormat = bits[format];
	if (stride)
		data->Stride = stride;
	else
		data->Stride = (INT)((width * (data->PixelFormat >> 8) + 31) & ~31) >> 3;
	SetAlphaFlag(data, isAlpha);
}
//---------------------------------------------------------------------------

// 用给定的宽度、高度和像素位数制造并返回新的与GDI+兼容的位图数据结构。
// 必须用FreeBitmapData释放
FORCEINLINE
VOID GetBitmapData(INT width, INT height, ImagePixelFormat format, BitmapData *data)
{
	GetBitmapData(width, height, 0, NULL, format, format == PixelFormat32bit, data);
	data->Scan0 = GlobalLock(GlobalAlloc(GHND, data->Height * data->Stride));
	if (data->Scan0) data->Reserved |= ScanAllocFlag;
}
//---------------------------------------------------------------------------

// 用给定的宽度和高度制造并返回新的与GDI+兼容的32位位图数据结构。
// 必须用FreeBitmapData释放
FORCEINLINE
VOID GetBitmapData(INT width, INT height, BitmapData *data)
{
	GetBitmapData(width, height, PixelFormat32bit, data);
}
//---------------------------------------------------------------------------

// 获取32位子位图数据结构
FORCEINLINE
BOOL GetBitmapData(CONST BitmapData *data, INT x, INT y, INT width, INT height, BitmapData *sub)
{
	width += x;
	height += y;
	if (width > (INT)data->Width)
		width = data->Width;
	if (height > (INT)data->Height)
		height = data->Height;
	INT ScanOffset = 0;
	if (x > 0)
	{
		width -= x;
		ScanOffset += (x << 2);
	}
	if (y > 0)
	{
		height -= y;
		ScanOffset += (y * data->Stride);
	}
	sub->Scan0 = (LPBYTE)data->Scan0 + ScanOffset;
	if (width <= 0 || height <= 0)
		return FALSE;
	sub->Width = width;
	sub->Height = height;
	sub->Stride = data->Stride;
	sub->PixelFormat = data->PixelFormat;
	sub->Reserved = data->Reserved & ~ScanAllocFlag;
	return TRUE;
}
//---------------------------------------------------------------------------

// 如果data分配了扫描线内存，释放扫描线内存
FORCEINLINE
VOID FreeBitmapData(BitmapData *data)
{
	if ((data->Reserved & ScanAllocFlag) && data->Scan0)
	{
		HGLOBAL handle = GlobalHandle(data->Scan0);
		if (handle)
		{
			GlobalUnlock(handle);
			GlobalFree(handle);
		}
		data->Reserved = 0;
	}
}
//---------------------------------------------------------------------------

// 获取32位位图数据拷贝参数

// 参数：目标数据，源数据，宽度，高度，目标扫描线，源扫描线，目标偏移，源偏移

FORCEINLINE
VOID GetDataCopyParams(CONST BitmapData *dest, CONST BitmapData *source,
	UINT &width, UINT &height, PARGBQuad &dstScan0, PARGBQuad &srcScan0,
	INT &dstOffset, INT &srcOffset)
{

	width = dest->Width < source->Width ? dest->Width : source->Width;

	height = dest->Height < source->Height ? dest->Height : source->Height;
	dstScan0 = (PARGBQuad)dest->Scan0;
	srcScan0 = (PARGBQuad)source->Scan0;

	dstOffset = (dest->Stride >> 2) - (INT)width;
	srcOffset = (source->Stride >> 2) - (INT)width;
}
//---------------------------------------------------------------------------

// 获取并返回32位位图数据结构data的边框扩展图像数据结构。Radius：扩展半径
FORCEINLINE
VOID GetExpendData(CONST BitmapData *data, UINT radius, BitmapData *exp)
{
	GetBitmapData(data->Width + (radius << 1), data->Height + (radius << 1),
		PixelFormat32bit, exp);
	SetAlphaFlag(exp, HasAlphaFlag(data));
	BitmapData sub;
	PARGBQuad pd, ps;
	UINT width, height;
	INT dstOffset, srcOffset;
	GetBitmapData(exp, radius, radius, data->Width, data->Height, &sub);
	GetDataCopyParams(&sub, data, width, height, pd, ps, dstOffset, srcOffset);
	PARGBQuad pt = pd - radius;
	UINT x, y;
	// 如果图像数据含Alpha，转换为PARGB像素格式
	if (HasAlphaFlag(data))
	{
		for (y = 0; y < height; y++, pd += dstOffset, ps += srcOffset)
		{
			for (x = 0; x < width; x++, pd++, ps++)
			{
				pd->Blue = (ps->Blue * ps->Alpha + 127) / 255;
				pd->Green = (ps->Green * ps->Alpha + 127) / 255;
				pd->Red = (ps->Red * ps->Alpha + 127) / 255;
				pd->Alpha = ps->Alpha;
			}
		}
	}
	// 否则, 直接像素拷贝
	else
	{
		for (y = 0; y < height; y++, pd += dstOffset, ps += srcOffset)
		{
			for (x = 0; x < width; *pd++ = *ps++, x++);
		}
	}
	// 扩展左右边框像素
	for (y = 0, pd = pt; y < height; y++)
	{
		for (x = 0, ps = pd + radius; x < radius; *pd++ = *ps, x++);
		for (x = 0, pd += width, ps = pd - 1; x < radius; *pd++ = *ps, x++);
	}
	// 扩展头尾边框像素
	PARGBQuad pb = (PARGBQuad)((LPBYTE)pd - exp->Stride);
	PARGBQuad pd0 = (PARGBQuad)exp->Scan0;
	for (y = 0; y < radius; y++)
	{
		for (x = 0; x < exp->Width; *pd0++ = pt[x], *pd++ = pb[x], x++);
	}
}
//---------------------------------------------------------------------------

// PARGB格式转换成ARGB格式
FORCEINLINE
VOID PArgbConvertArgb(BitmapData *data)
{
	PARGBQuad p = (PARGBQuad)data->Scan0;
	INT dataOffset = (data->Stride >> 2) - (INT)data->Width;
	for (UINT y = 0; y < data->Height; y++, p += dataOffset)
	{
		for (UINT x = 0; x < data->Width; x++, p++)
		{
			p->Blue = p->Blue * 255 / p->Alpha;
			p->Green = p->Green * 255 / p->Alpha;
			p->Red = p->Red * 255 / p->Alpha;
		}
	}
}
//---------------------------------------------------------------------------

// ARGB格式转换成PARGB格式
FORCEINLINE
VOID ArgbConvertPArgb(BitmapData *data)
{
	PARGBQuad p = (PARGBQuad)data->Scan0;
	INT dataOffset = (data->Stride >> 2) - (INT)data->Width;
	for (UINT y = 0; y < data->Height; y++, p += dataOffset)
	{
		for (UINT x = 0; x < data->Width; x++, p++)
		{
			p->Blue = (p->Blue * p->Alpha + 127) / 255;
			p->Green = (p->Green * p->Alpha + 127) / 255;
			p->Red = (p->Red * p->Alpha + 127) / 255;
		}
	}
}
//---------------------------------------------------------------------------


#endif