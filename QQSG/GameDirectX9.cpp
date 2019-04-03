#include "GameDirectX9.h"

DirectX92D::DirectX92D() {
	Directx9 = nullptr;
	D3DSurface9 = nullptr;
	D3DDevice9 = nullptr;
	Dxline = nullptr;
	D3DSprite = nullptr;
}
DirectX92D::~DirectX92D() {
	//资源释放;
	if (D3DSurface9)D3DSurface9->Release();
	if (D3DDevice9)D3DDevice9->Release();
	if (Directx9)Directx9->Release();
	if (Dxline)Dxline->Release();
	if (D3DSprite)D3DSprite->Release();
}
//初始化Directx9
BOOL DirectX92D::InitD3D(HWND hWnd,int Width,int Height,bool FullScreen) {
	//创建D3D设备;
	Directx9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!Directx9)return false;
	D3DPRESENT_PARAMETERS D3D9PresentParam = { 0 };
	D3D9PresentParam.hDeviceWindow = hWnd;
	D3D9PresentParam.Windowed = !FullScreen;
	D3D9PresentParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
	D3D9PresentParam.EnableAutoDepthStencil = 1;
	D3D9PresentParam.AutoDepthStencilFormat = D3DFMT_D24S8;
	D3D9PresentParam.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	D3D9PresentParam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	D3D9PresentParam.BackBufferCount = 1;
	D3D9PresentParam.BackBufferFormat = D3DFMT_X8R8G8B8;
	D3D9PresentParam.BackBufferWidth = Width;
	D3D9PresentParam.BackBufferHeight = Height;
	D3D9PresentParam.MultiSampleQuality = 0;
	D3D9PresentParam.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
	//创建D3D驱动;
	Directx9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING, &D3D9PresentParam, &D3DDevice9);
	if (!D3DDevice9)return false;
	//获取缓冲区指针;
	D3DDevice9->GetBackBuffer(NULL, NULL, D3DBACKBUFFER_TYPE_MONO, &D3DSurface9);
	if (!D3DSurface9)return false;
	//创建直线绘制对象;
	D3DXCreateLine(D3DDevice9, &Dxline);
	if (!Dxline)return false;
	//创建2D精灵对象;
	D3DXCreateSprite(D3DDevice9, &D3DSprite);
	if (!D3DSprite)return FALSE;

	return true;

}
//清理设备;
HRESULT DirectX92D::Clear(DWORD Count, D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
	return D3DDevice9->Clear(Count, pRects, Flags, Color, Z, Stencil);
}
//获取Directx9对象;
LPDIRECT3D9 DirectX92D::GetDirectx9() {
	return Directx9;
}
//获取D3Dx9设备对象;
LPDIRECT3DDEVICE9 DirectX92D::GetD3Devicex9() {
	return D3DDevice9;
}
//获取D3Dx9缓冲区对象;
LPDIRECT3DSURFACE9 DirectX92D::GetD3DSurfacex9() {
	return D3DSurface9;
}
//获取图片信息;
D3DXIMAGE_INFO DirectX92D::GetImageInfo(const wchar_t *filename) {
	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename, &info);
	CopyMemory(&Imginfo, &info, sizeof(D3DXIMAGE_INFO));
	return Imginfo;
}
//获取图片信息;
D3DXIMAGE_INFO DirectX92D::GetImageInfoInMemory(void *SrcData, UINT SrcLen) {
	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFileInMemory(SrcData, SrcLen, &info);
	CopyMemory(&Imginfo, &info, sizeof(D3DXIMAGE_INFO));
	return Imginfo;
}
//从内存中加载纹理;
LPDIRECT3DTEXTURE9 DirectX92D::LoadMemTexture(void* ImgStream, UINT Imglen, D3DCOLOR transcolor)
{
	if (!ImgStream)return nullptr;
	LPDIRECT3DTEXTURE9 texture = NULL;
	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFileInMemory(ImgStream, Imglen, &info);
	if (result != D3D_OK) { MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return nullptr; }
	//create the new texture by loading a bitmap image file
	result = D3DXCreateTextureFromFileInMemoryEx(D3DDevice9, ImgStream, Imglen,
		info.Width,            //bitmap image width
		info.Height,           //bitmap image height
		1,                     //mip-map levels (1 for no chain)
		D3DPOOL_DEFAULT,       //the type of surface (standard)
		D3DFMT_UNKNOWN,        //surface format (default)
		D3DPOOL_DEFAULT,       //memory class for the texture
		D3DX_DEFAULT,          //image filter
		D3DX_DEFAULT,          //mip filter
		transcolor,            //color key for transparency
		&info,                 //bitmap file info (from loaded file)
		NULL,                  //color palette
		&texture);            //destination texture

	//make sure the bitmap textre was loaded correctly
	if (result != D3D_OK) { MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return nullptr; }
	return texture;
}
//从文件中加载纹理
LPDIRECT3DTEXTURE9 DirectX92D::LoadTexture(const wchar_t* filename, D3DCOLOR transcolor)
{
	if (!filename)return nullptr;
	/*WCHAR ExePath[256] = { 0 };
	GetExePathW(ExePath, sizeof(ExePath));
	wcscat(ExePath, filename);*/
	LPDIRECT3DTEXTURE9 texture = NULL;
	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename, &info);
	if (result != D3D_OK) { MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return nullptr; }
	//create the new texture by loading a bitmap image file
	result = D3DXCreateTextureFromFileEx(
		D3DDevice9,                //Direct3D device object
		filename,      //bitmap filename
		info.Width,            //bitmap image width
		info.Height,           //bitmap image height
		1,                     //mip-map levels (1 for no chain)
		D3DPOOL_DEFAULT,       //the type of surface (standard)
		D3DFMT_UNKNOWN,        //surface format (default)
		D3DPOOL_DEFAULT,       //memory class for the texture
		D3DX_DEFAULT,          //image filter
		D3DX_DEFAULT,          //mip filter
		transcolor,            //color key for transparency
		&info,                 //bitmap file info (from loaded file)
		NULL,                  //color palette
		&texture);            //destination texture
	
	//make sure the bitmap textre was loaded correctly
	if (result != D3D_OK) { MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return nullptr; }
	return texture;
}
//从文件中加载自定义大小纹理;
LPDIRECT3DTEXTURE9 DirectX92D::LoadRectTexture(const wchar_t* filename, UINT Width, UINT Height, D3DCOLOR transcolor) {
	LPDIRECT3DTEXTURE9 texture = NULL;
	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename, &info);
	if (result != D3D_OK) { MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return nullptr; }
	//create the new texture by loading a bitmap image file
	result = D3DXCreateTextureFromFileEx(
		D3DDevice9,                //Direct3D device object
		filename,      //bitmap filename
		Width,            //bitmap image width
		Height,           //bitmap image height
		1,                     //mip-map levels (1 for no chain)
		D3DPOOL_DEFAULT,       //the type of surface (standard)
		D3DFMT_UNKNOWN,        //surface format (default)
		D3DPOOL_DEFAULT,       //memory class for the texture
		D3DX_DEFAULT,          //image filter
		D3DX_DEFAULT,          //mip filter
		transcolor,            //color key for transparency
		&info,                 //bitmap file info (from loaded file)
		NULL,                  //color palette
		&texture);            //destination texture

	//make sure the bitmap textre was loaded correctly
	if (result != D3D_OK) { MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return nullptr; }
	return texture;
}
//绘制一个材质;(材质,x,y,宽度,高度,缩放比例,旋转方向,颜色)
BOOL DirectX92D::DrawTexture(LPDIRECT3DTEXTURE9 texture, float x, float y, UINT width, UINT height, float scale, float rotation, D3DCOLOR color)
{
	if (!texture)return FALSE;
	//create a scale vector
	D3DXVECTOR2 scalec(scale, scale);
	//create a translate vector
	D3DXVECTOR2 trans(x, y);
	//set center by dividing width and height by two
	D3DXVECTOR2 center((float)(width * scale) / 2, (float)(height * scale) / 2);
	//create 2D transformation matrix
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scalec, &center, rotation, &trans);
	//tell sprite object to use the transform
	D3DSprite->SetTransform(&mat);
	D3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
	//draw the sprite frame
	D3DSprite->Draw(texture, NULL, NULL, NULL, color);
	D3DXMatrixIdentity(&mat);
	D3DSprite->SetTransform(&mat);
	D3DSprite->End();
	return TRUE;
}
//绘制材质纹理帧;
BOOL DirectX92D::DrawAnimateTexture(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, int frame, int columns, float scale, float rotation, D3DCOLOR color)
{
	//create a scale vector
	D3DXVECTOR2 scalec(scale, scale);
	//create a translate vector
	D3DXVECTOR2 trans((float)x, (float)y);
	//set center by dividing width and height by two
	D3DXVECTOR2 center((float)(width * scale) / 2, (float)(height * scale) / 2);
	//create 2D transformation matrix
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scalec, &center, rotation, &trans);
	//tell sprite object to use the transform
	//创建2D精灵对象;
	LPD3DXSPRITE D3DSprite = nullptr;
	D3DXCreateSprite(D3DDevice9, &D3DSprite);
	if (!D3DSprite)return FALSE;
	D3DSprite->SetTransform(&mat);
	//calculate frame location in source image
	int fx = (frame % columns) * width;
	int fy = (frame / columns) * height;
	RECT srcRect = { fx, fy, fx + width, fy + height };
	//draw the sprite frame
	D3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
	D3DSprite->Draw(image, &srcRect, NULL, NULL, color);
	D3DXMatrixIdentity(&mat);
	D3DSprite->SetTransform(&mat);
	D3DSprite->End();
	D3DSprite->Release();
	return TRUE;
}
//绘制图片中心点材质;
BOOL DirectX92D::DrawImageCenterTexture(const wchar_t *ImageName, int x, int y, int width, int height, float scale, float rotation, D3DCOLOR Imagecolor, D3DCOLOR Drawcolor) {
	LPDIRECT3DTEXTURE9 texture = NULL;
	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(ImageName, &info);
	if (result != D3D_OK) { 
		MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return FALSE; }
	//create the new texture by loading a bitmap image file
	D3DXCreateTextureFromFileEx(
		D3DDevice9,                //Direct3D device object
		ImageName,      //bitmap filename
		info.Width,            //bitmap image width
		info.Height,           //bitmap image height
		1,                     //mip-map levels (1 for no chain)
		D3DPOOL_DEFAULT,       //the type of surface (standard)
		D3DFMT_UNKNOWN,        //surface format (default)
		D3DPOOL_DEFAULT,       //memory class for the texture
		D3DX_DEFAULT,          //image filter
		D3DX_DEFAULT,          //mip filter
		Imagecolor,            //color key for transparency
		&info,                 //bitmap file info (from loaded file)
		NULL,                  //color palette
		&texture);            //destination texture

	//make sure the bitmap textre was loaded correctly
	if (result != D3D_OK) { MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return FALSE; }
	//create a scale vector
	D3DXVECTOR2 scalec(scale, scale);
	//create a translate vector
	D3DXVECTOR2 trans((float)x - (info.Width / 2), (float)y - (info.Height / 2));
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scalec, NULL, rotation, &trans);
	//tell sprite object to use the transform
	//创建2D精灵对象;
	LPD3DXSPRITE D3DSprite = nullptr;
	D3DXCreateSprite(D3DDevice9, &D3DSprite);
	if (!D3DSprite)return FALSE;
	D3DSprite->SetTransform(&mat);
	D3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
	//draw the sprite frame
	D3DSprite->Draw(texture, NULL, NULL, NULL, Drawcolor);
	D3DXMatrixIdentity(&mat);
	D3DSprite->SetTransform(&mat);
	D3DSprite->End();
	D3DSprite->Release();
	texture->Release();
	return TRUE;
}
//绘制图片自定义大小材质;
BOOL DirectX92D::DrawImageRectTexture(const wchar_t *ImageName, int x, int y, int width, int height, float scale, float rotation, D3DCOLOR Imagecolor, D3DCOLOR Drawcolor) {
	LPDIRECT3DTEXTURE9 texture = NULL;
	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(ImageName, &info);
	if (result != D3D_OK) { MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return FALSE; }
	//create the new texture by loading a bitmap image file
	D3DXCreateTextureFromFileEx(
		D3DDevice9,                //Direct3D device object
		ImageName,      //bitmap filename
		width,            //bitmap image width
		height,           //bitmap image height
		1,                     //mip-map levels (1 for no chain)
		D3DPOOL_DEFAULT,       //the type of surface (standard)
		D3DFMT_UNKNOWN,        //surface format (default)
		D3DPOOL_DEFAULT,       //memory class for the texture
		D3DX_DEFAULT,          //image filter
		D3DX_DEFAULT,          //mip filter
		Imagecolor,            //color key for transparency
		&info,                 //bitmap file info (from loaded file)
		NULL,                  //color palette
		&texture);            //destination texture

	//make sure the bitmap textre was loaded correctly
	if (result != D3D_OK) { MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return FALSE; }
	//create a scale vector
	D3DXVECTOR2 scalec(scale, scale);
	//create a translate vector
	D3DXVECTOR2 trans((float)x, (float)y);
	D3DXVECTOR2 center((float)(width * scale) / 2, (float)(height * scale) / 2);
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scalec, &center, rotation, &trans);
	//tell sprite object to use the transform
	//创建2D精灵对象;
	LPD3DXSPRITE D3DSprite = nullptr;
	D3DXCreateSprite(D3DDevice9, &D3DSprite);
	if (!D3DSprite)return FALSE;
	D3DSprite->SetTransform(&mat);
	D3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
	//draw the sprite frame
	D3DSprite->Draw(texture, NULL, NULL, NULL, Drawcolor);
	D3DXMatrixIdentity(&mat);
	D3DSprite->SetTransform(&mat);
	D3DSprite->End();
	D3DSprite->Release();
	texture->Release();
	return TRUE;
}
//绘制图片材质;
BOOL DirectX92D::DrawImageTexture(const wchar_t *ImageName, int x, int y, int width, int height, float scale, float rotation, D3DCOLOR Imagecolor, D3DCOLOR Drawcolor) {
	LPDIRECT3DTEXTURE9 texture = NULL;
	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(ImageName, &info);
	if (result != D3D_OK) { MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return FALSE; }
	//create the new texture by loading a bitmap image file
	D3DXCreateTextureFromFileEx(
		D3DDevice9,                //Direct3D device object
		ImageName,      //bitmap filename
		info.Width,            //bitmap image width
		info.Height,           //bitmap image height
		1,                     //mip-map levels (1 for no chain)
		D3DPOOL_DEFAULT,       //the type of surface (standard)
		D3DFMT_UNKNOWN,        //surface format (default)
		D3DPOOL_DEFAULT,       //memory class for the texture
		D3DX_DEFAULT,          //image filter
		D3DX_DEFAULT,          //mip filter
		Imagecolor,            //color key for transparency
		&info,                 //bitmap file info (from loaded file)
		NULL,                  //color palette
		&texture);            //destination texture

	//make sure the bitmap textre was loaded correctly
	if (result != D3D_OK) { MessageBox(nullptr, L"资源加载失败,请确认资源文件是否损坏!", nullptr, MB_OK); ExitProcess(NULL); return FALSE; }
	//create a scale vector
	D3DXVECTOR2 scalec(scale, scale);
	//create a translate vector
	D3DXVECTOR2 trans((float)x, (float)y);
	D3DXVECTOR2 center((float)(width * scale) / 2, (float)(height * scale) / 2);
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scalec, &center, rotation, &trans);
	//tell sprite object to use the transform
	//创建2D精灵对象;
	LPD3DXSPRITE D3DSprite = nullptr;
	D3DXCreateSprite(D3DDevice9, &D3DSprite);
	if (!D3DSprite)return FALSE;
	D3DSprite->SetTransform(&mat);
	D3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
	//draw the sprite frame
	D3DSprite->Draw(texture, NULL, NULL, NULL, Drawcolor);
	D3DXMatrixIdentity(&mat);
	D3DSprite->SetTransform(&mat);
	D3DSprite->End();
	D3DSprite->Release();
	texture->Release();
	return TRUE;
}
//绘制直线;
BOOL DirectX92D::DrawLineto(float x1, float y1, float x2, float y2, float Width, D3DCOLOR Color) {
	D3DXVECTOR2 Cline[2];
	Cline[0] = D3DXVECTOR2(x1, y1);
	Cline[1] = D3DXVECTOR2(x2, y2);
	Dxline->SetWidth(Width);
	Dxline->SetAntialias(TRUE);
	Dxline->Begin();
	Dxline->Draw(Cline, 2, Color);
	Dxline->End();
	return TRUE;
}
//绘制字体;
BOOL DirectX92D::DrawFont(const char *OutFont, int FontSize, int Weight, BOOL ltalic, const char *FontName, LPRECT rect, D3DCOLOR Color) {
	D3DXFONT_DESCA D3dfont = { 0 };
	D3dfont.Height = FontSize + 1;
	D3dfont.Width = 0;
	D3dfont.Weight = Weight;
	D3dfont.MipLevels = D3DX_DEFAULT;
	D3dfont.Italic = ltalic;
	D3dfont.CharSet = DEFAULT_CHARSET;
	D3dfont.OutputPrecision = 0;
	D3dfont.Quality = 0;
	D3dfont.PitchAndFamily = 0;
	strcpy(D3dfont.FaceName, FontName);
	LPD3DXFONT DxFont = nullptr;
	D3DXCreateFontIndirectA(D3DDevice9, &D3dfont, &DxFont);
	if (!DxFont)return FALSE;
	DxFont->DrawTextA(NULL, OutFont, -1, rect, DT_TOP | DT_LEFT, Color);
	DxFont->Release();
	return TRUE;
}
//绘制矩形;
void DirectX92D::DrawRectagle(float left, float top, float Right, float bottom, float Width, D3DCOLOR Color) {
	DrawLineto(left, top, Right, top, Width, Color);
	DrawLineto(left, top, left, bottom, Width, Color);
	DrawLineto(Right, top, Right, bottom, Width, Color);
	DrawLineto(left, bottom, Right, bottom, Width, Color);
}
//绘制文本字体;
BOOL DirectX92D::DrawTextFont(const char *OutFont, int FontSize, int Weight, BOOL ltalic, const char *FontName, LPRECT rect, D3DCOLOR Color) {
	LPD3DXFONT pFont = nullptr;
	D3DXCreateFontA(D3DDevice9, FontSize, 0, Weight, 0, ltalic, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FontName, &pFont);
	if (!pFont)return FALSE;
	pFont->DrawTextA(NULL, OutFont, -1, rect, DT_NOCLIP | DT_LEFT, Color);
	pFont->Release();
	return TRUE;
}

