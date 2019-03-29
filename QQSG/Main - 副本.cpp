#include "Main.h"

const WCHAR ClassName[] = L"QQSGDiy";
const WCHAR StringBGM1[] = L"Music\\BGM1.mp3";
WCHAR ExePath[256] = { 0 }, StrBGM1[256] = { 0 };
HWND G_hWnd;
HSTREAM StreamBGM1;
LPDIRECT3DTEXTURE9 texture;
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	WNDCLASS WinClass = { 0 };
	WinClass.hInstance = hInstance;
	WinClass.lpszClassName = ClassName;
	WinClass.lpfnWndProc = WndProc;
	WinClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WinClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WinClass.style = CS_HREDRAW | CS_VREDRAW;
	WinClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	int WinX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (CL_WinWidth / 2),
		WinY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (CL_WinHeight / 2);
	if (!RegisterClass(&WinClass))MessageBox(0, L"注册窗口类名失败!", NULL, MB_OK);
	G_hWnd = CreateWindow(ClassName, L"这是假三国单机版", 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
		WinX, WinY, CL_WinWidth, CL_WinHeight, NULL, NULL, hInstance, NULL);
	if (!G_hWnd)MessageBox(0, L"创建窗口失败!", NULL, MB_OK);
	ShowWindow(G_hWnd, SW_SHOW);
	UpdateWindow(G_hWnd);
	MSG msg = { 0 };
	while (GetMessage(&msg,G_hWnd,NULL,NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	char path[256] = { 0 };
	ResouceDataFile ResData;
	ResouceDataFile::ResDataInfo *ResFa = nullptr;
	int RNumber = 0;
	void *Fir = nullptr, *Lor = nullptr;
	FILE *filr = nullptr;
	DWORD ImgWidth = 0, ImgHeight = 0,bu[17];
	DWORD *LKo;
	int Hr = 0,ARG2 = 0, ARG1 = 0;;
	switch (uMsg)
	{
	case WM_CREATE:{
		GetExePathW(ExePath, sizeof(ExePath));
		if (!InitD3D(hWnd, CL_WinWidth, CL_WinHeight, false)) { MessageBox(hWnd, L"初始化D3D失败!", nullptr, MB_OK); ExitProcess(NULL); }
		if (!BASS_Init(-1, 44100, BASS_DEVICE_3D, hWnd, NULL)) { MessageBox(hWnd, L"初始化BASS音频库失败!", nullptr, MB_OK); ExitProcess(NULL); }
		CopyMemory((void*)StrBGM1, ExePath, wcslen(ExePath) * sizeof(WCHAR));
		wcscat(StrBGM1, StringBGM1);
		StreamBGM1 = BASS_StreamCreateFile(FALSE, StrBGM1, NULL, NULL, BASS_SAMPLE_LOOP);//循环播放
		if (!StreamBGM1) { MessageBox(hWnd, L"音频打开失败,请确认文件是否丢失!", nullptr, MB_OK); ExitProcess(NULL); }
		BASS_ChannelPlay(StreamBGM1, TRUE);
		texture = LoadTexture(L"1-1.bmp");
		strcat(path, "E:\\ProjectCode\\C&C++\\Game\\QQSG\\Debug\\update.pkg");
		RNumber = GetResData(path, (void**)&ResFa);
		Fir = LocalAlloc(LMEM_ZEROINIT, ResFa[1].pOriginSize);
		Lor = LocalAlloc(LMEM_ZEROINIT, ResFa[1].pDataSize);
		filr = fopen(path, "rb");
		fseek(filr, ResFa[1].pOffset, SEEK_SET);
		fread(Lor, ResFa[1].pDataSize, 1, filr);
		uncompress((Bytef*)Fir, (uLongf*)&ResFa[1].pOriginSize, (Bytef*)Lor, ResFa[1].pDataSize);
		ImgWidth = *(DWORD*)((DWORD*)Fir + 16);
		ImgHeight = *(DWORD*)((DWORD*)Fir + 17);
		LKo = (DWORD*)Fir;
		LKo = (DWORD*)((char*)LKo + 8);
		for (int i = 0; i < 17; i++) {
			bu[i] = *LKo;
			LKo = (DWORD*)((char*)LKo + 4);
			/*ARG1 = *(DWORD*)((DWORD*)Fir + 18 + (i * 18));
			ARG2 = *(DWORD*)((DWORD*)Fir + 19 + (i * 18));
			Hr = ARG(ARG1, ARG2);
			if (Hr != 0) {
				Hr = 1000;
			}*/
		}
		ImgWidth = bu[4];
		ImgHeight = bu[5];
		BYTE *Imgdata = nullptr;
		int len = (ImgWidth*ImgHeight);
		
		/*for (int i = 0; i < len; i++) {
			bu[6] = *LKo;
			LKo = (DWORD*)((char*)LKo + 1);
			bu[7] = *LKo;
			LKo = (DWORD*)((char*)LKo + 1);
			Imgdata[i] = ARG(bu[6], bu[7]);
		}*/
		fclose(filr);
		CopyMemory(path, "E:\\ProjectCode\\C&C++\\Game\\QQSG\\Debug\\1-1.bmp", 48);
		filr = fopen(path, "rb");
		fseek(filr, 0, SEEK_END);
		len = ftell(filr);
		fseek(filr, 0, SEEK_SET);
		Imgdata = (BYTE*)LocalAlloc(LMEM_ZEROINIT, len);
		fread(Imgdata, len, 1, filr);
		texture = LoadMemTexture(Imgdata, len);
		//fwrite(Imgdata, len,1, filr);
		fclose(filr);
		//LocalFree(Imgdata);
		/*for (int i = 0; i < RNumber; i++) {
			OutputDebugStringA((char*)ResFa[i].pPath);
			OutputDebugStringA("\n");
			char write[256] = "E:\\ProjectCode\\C&C++\\Game\\QQSG\\Debug\\New\\";
			Fir = LocalAlloc(LMEM_ZEROINIT, ResFa[i].pOriginSize);
			Lor = LocalAlloc(LMEM_ZEROINIT, ResFa[i].pDataSize);
			filr = fopen(path, "rb");
			fseek(filr, ResFa[i].pOffset, SEEK_SET);
			fread(Lor, ResFa[i].pDataSize, 1, filr);
			uncompress((Bytef*)Fir, (uLongf*)&ResFa[i].pOriginSize, (Bytef*)Lor, ResFa[i].pDataSize);
			fclose(filr);
			//GetExePathA(write, sizeof(write));
			strcat(write, (char*)ResFa[i].pPath);
			CreateDirectoryEc(write);
			filr = fopen(write, "w+");
			if (!filr) {
				LocalFree(Fir);
				LocalFree(Lor);
				LocalFree(ResFa[i].pPath);
				continue; }
			else {
				fwrite(Fir, ResFa[i].pOriginSize, 1, filr);
				fclose(filr);
				LocalFree(Fir);
				LocalFree(Lor);
				LocalFree(ResFa[i].pPath);
			}
		}*/
		LocalFree(ResFa);
	}
		break;
	case WM_CLOSE:
		BASS_Free();
		ExitProcess(0);
		break;
	case WM_PAINT:
		D3DGameRun();
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void D3DGameRun() {
	if (!D3DDevice9)return;
	D3DDevice9->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	if (SUCCEEDED(D3DDevice9->BeginScene())) {
		D3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
		DrawAnimateTexture(texture, 300,300,100,300,0,2);
		DrawTexture(texture, 100, 100, 50, 50);
		D3DSprite->End();
		D3DDevice9->EndScene();
		D3DDevice9->Present(NULL, NULL, NULL, NULL);
	}

}
void DrawLoginBackground() {

}
