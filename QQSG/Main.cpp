#include "Main.h"
using namespace ResouceFile;
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	WNDCLASS WinClass = { 0 };
	WinClass.hInstance = hInstance;
	WinClass.lpszClassName = ClassName;
	WinClass.lpfnWndProc = WndProc;
	WinClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WinClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WinClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	WinClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	int WinX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (CL_WinWidth / 2),
		WinY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (CL_WinHeight / 2);
	if (!RegisterClass(&WinClass))MessageBox(0, L"注册窗口类名失败!", NULL, MB_OK);
	G_hWnd = CreateWindow(ClassName, L"这是假三国单机版", 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		WinX, WinY, CL_WinWidth, CL_WinHeight, NULL, NULL, hInstance, NULL);
	if (!G_hWnd)MessageBox(0, L"创建窗口失败!", NULL, MB_OK);
	ShowWindow(G_hWnd, SW_SHOWNORMAL);
	UpdateWindow(G_hWnd);
	MSG msg = { 0 };
	while (GetMessage(&msg,NULL,NULL,NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	char path[256] = { 0 };
	switch (uMsg)
	{
	case WM_CREATE: {
		//CoInitialize(nullptr);
		Ime.SetMaxLen(14);
		GetExePath(ExePath, sizeof(ExePath));
		//初始化程序;
		if (!D2Dx9.InitD3D(hWnd, CL_WinWidth, CL_WinHeight, false)) { MessageBox(hWnd, L"初始化D3D失败!", nullptr, MB_OK); ExitProcess(NULL); }
		if (!BASS_Init(-1, 44100, BASS_DEVICE_3D, hWnd, NULL)) { MessageBox(hWnd, L"初始化BASS音频库失败!", nullptr, MB_OK); ExitProcess(NULL); }
		LPDIRECT3DDEVICE9 D3DDevice9 = D2Dx9.GetD3Devicex9();
		D3DDevice9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		D3DDevice9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		D3DDevice9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		D3DDevice9->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);
		//播放BGM;
		PlayerBGM_Login("Login1.mp3");
		//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InitDrawCreatePlayer, NULL, NULL, NULL);
		InitDrawCreatePlayer();
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)D3DGameRun, NULL, NULL, NULL);
		/*strcat(path, "E:\\ProjectCode\\C&C++\\Game\\QQSG\\Debug\\ResData.db");
		char IT[512] = "E:\\ProjectCode\\C&C++\\Game\\QQSG\\Debug\\ResData.db";
		int ilen = compressBound(strlen(IT));
		char OT[1024] = { 0 };
		compress((Bytef*)OT, (uLongf*)&ilen, (Bytef*)IT, 1024);
		char OiT[1024] = { 0 };
		int olen = 1024;
		uncompress((Bytef*)OiT, (uLongf*)&olen, (Bytef*)OT, ilen);
		RNumber = ResData.GetResDataIndex(path, (void**)&ResFa);
		/ *Fir = LocalAlloc(LMEM_ZEROINIT, ResFa[1].pOriginSize);
		Lor = LocalAlloc(LMEM_ZEROINIT, ResFa[1].pDataSize);
		filr = fopen(path, "rb");
		fseek(filr, ResFa[1].pOffset, SEEK_SET);
		fread(Lor, ResFa[1].pDataSize, 1, filr);
		uncompress((Bytef*)Fir, (uLongf*)&ResFa[1].pOriginSize, (Bytef*)Lor, ResFa[1].pDataSize);* /
		//fclose(filr);
		//LocalFree(Imgdata);
		for (int i = 0; i < RNumber; i++) {
			OutputDebugStringA((char*)ResFa[i].pFileName);
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
			strcat(write, (char*)ResFa[i].pFileName);
			CreateDirectoryEc(write);
			filr = fopen(write, "wb");
			if (!filr) {
				LocalFree(Fir);
				LocalFree(Lor);
				LocalFree(ResFa[i].pFileName);
				continue; }
			else {
				fwrite(Fir, ResFa[i].pOriginSize, 1, filr);
				fclose(filr);
				LocalFree(Fir);
				LocalFree(Lor);
				LocalFree(ResFa[i].pFileName);
			}
		}
		LocalFree(ResFa);
	}*/
		break;
	}
	case WM_CLOSE:
		BASS_Free();
		ExitProcess(0);
		break;
	case WM_KEYDOWN:
		Ime.SetKeyState(TRUE);
		ImeEvent(wParam);
		if (LOWORD(wParam) == VK_UP) {
			Countf++;
		}
		if (LOWORD(wParam) == VK_DOWN) {
			Countf--;
		}
		break;
	case WM_KEYUP:
		Ime.SetKeyState(FALSE);
		break;
	case WM_MOUSEMOVE:
		MousePoint.KeyCode = wParam;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONDOWN:
		MousePoint.KeyCode = wParam;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_RBUTTONDOWN:
		MousePoint.KeyCode = wParam;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONUP:
		MousePoint.KeyCode = wParam;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_RBUTTONUP:
		MousePoint.KeyCode = wParam;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
//输入法按键处理;
void ImeEvent(WPARAM wParam) {
	int Keych = LOWORD(wParam);
	if (Keych == '\b') {
		Ime.DeleteImeStr();
	}
	else {
		char chc[3] = { 0 };
		if (GetKeyState(VK_CAPITAL)) {
			chc[0] = Keych;
		}
		else {
			if (Keych >= 'A' && Keych <= 'Z') {
				chc[0] = Keych + 32;
			}
			else if (Keych >= VK_NUMPAD0 && Keych <= VK_NUMPAD9) {
				sprintf(chc, "%d", Keych - VK_NUMPAD0);
			}
			else if (Keych >= '0' && Keych <= '9') {
				sprintf(chc, "%d", Keych - '0');
			}
		}
		Ime.SetImeChar(chc);
	}
}
//播放登录背景音乐
void PlayerBGM_Login(const char *MusicName) {
	char path[256] = { 0 };
	ResouceDataFile ResData;
	ResouceDataFile::ResDataIndexInfo *ResFa = nullptr;
	int RNumber = 0;
	GetExePathA(path, MAX_PATH);
	strcat(path, "Music.db");
	ResFa = ResData.GetResDataIndex(path);
	RNumber = ResData.GetResIndexNumber();
	for (int i = 0; i < RNumber; i++) {
		//如果是登录1背景音乐,则播放;
		char *pFileName = (char*)ResFa[i].pFileName;
		if(!pFileName)continue;
		if (strcmp((char*)ResFa[i].pFileName, MusicName) == 0) {
			//申请内存音频流缓冲区
			BGM_Login1 = LocalAlloc(LMEM_ZEROINIT, ResFa[i].pOriginSize);
			if (!BGM_Login1) {
				MessageBox(0, L"内存不足,打开音频失败!", nullptr, MB_OK);
				ExitProcess(NULL);
			}
			//申请临时解压内存音频流缓冲区;
			void *TmpAlloc = LocalAlloc(LMEM_ZEROINIT, ResFa[i].pDataSize);
			if (!TmpAlloc) {
				MessageBox(0, L"内存不足,打开音频失败!", nullptr, MB_OK);
				ExitProcess(NULL);
			}
			//读取音频流数据存入内存
			FILE *pFile = fopen(path, "rb");
			if (!pFile) {
				MessageBox(0, L"音频打开失败,请确认文件是否丢失!", nullptr, MB_OK);
				ExitProcess(NULL);
			}
			fseek(pFile, ResFa[i].pOffset, SEEK_SET);
			fread(TmpAlloc, ResFa[i].pDataSize, 1, pFile);
			uLongf UnzipLen = ResFa[i].pDataSize;
			//解压音频数据流;
			uncompress((Bytef*)BGM_Login1, &UnzipLen, (Bytef*)TmpAlloc, ResFa[i].pDataSize);
			//释放申请的资源;
			LocalFree(TmpAlloc);
			fclose(pFile);
			StreamBGM1 = BASS_StreamCreateFile(TRUE, BGM_Login1, 0, UnzipLen, BASS_SAMPLE_LOOP);
			if (!StreamBGM1) {
				MessageBox(0, L"音频打开失败,请确认资源文件是否损坏!", nullptr, MB_OK);
				ExitProcess(NULL);
			}
			break;
		}
	}
	//播放音频
	BASS_ChannelPlay(StreamBGM1, TRUE);
}
//游戏绘制
void D3DGameRun() {
	while (DrawGame)
	{
		LPDIRECT3DDEVICE9 D3DDevice9 = D2Dx9.GetD3Devicex9();
		if (!D3DDevice9)return;
		D2Dx9.Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		if (SUCCEEDED(D3DDevice9->BeginScene())) {
			ShowFPS();
			//绘制提示框;
			if (DrawTip) {
				DrawTipBox();
			}
			if (DrawCreatPlyer) {
				DrawCreatePlayer();
			}
			D3DDevice9->EndScene();
			D3DDevice9->Present(NULL, NULL, NULL, NULL);
		}
		Sleep(1);
	}

}
//显示FPS帧率:
void ShowFPS() {
	//计算每秒FPS帧数;
	if (OldFpsTick == 0 || GetTickCount() - OldFpsTick > 1100) {
		OldFpsTick = GetTickCount();
		NewFPSCount = FPSCount;
		FPSCount = 0;
	}
	else FPSCount++;
	char FPSOut[50];
	sprintf(FPSOut, "FPS:%d", NewFPSCount);
	RECT lrect = { CL_WinWidth - 60,10,CL_WinWidth,50 };
	D2Dx9.DrawFont(FPSOut, 12, NULL, FALSE, "隶书", &lrect, D3DCOLOR_XRGB(255, 0, 0));
}
//绘制提示框;
void DrawTipBox() {
	UINT TipWidth = 500, TipHeight = 600, TipTop = 30;
	WCHAR szPath[MAX_PATH];
	GetExePath(szPath, sizeof(szPath));
	WCHAR szImg_Tip[MAX_PATH];
	wcscpy(szImg_Tip, szPath);
	wcscat(szImg_Tip, L"UIRes\\登录UI\\登录角色\\提示框-1.bmp");
	LPDIRECT3DTEXTURE9 TipBox = D2Dx9.LoadRectTexture(szImg_Tip, TipWidth, TipHeight);
	WCHAR SubmitT[3][256] = { L"UIRes\\登录UI\\登录角色\\确定-1.bmp",L"UIRes\\登录UI\\登录角色\\确定-2.bmp",L"UIRes\\登录UI\\登录角色\\确定-3.bmp" };
	LPDIRECT3DTEXTURE9 Submit[3] = { 0 };
	for (int i = 0; i < 3; i++) {
		wcscpy(szImg_Tip, szPath);
		wcscat(szImg_Tip, SubmitT[i]);
		Submit[i] = D2Dx9.LoadTexture(szImg_Tip);
	}
	D2Dx9.DrawTexture(TipBox, (float)TipWidth / 2, (float)TipTop);
	char TipT[] = "关于游戏:";
	RECT lrect = { (LONG)TipWidth - 70,(LONG)TipTop + 5,CL_WinWidth,CL_WinHeight };
	D2Dx9.DrawFont(TipT, 25, 10, TRUE, "黑体", &lrect, D3DCOLOR_XRGB(255, 0, 0));
	char TipText[] = "*.本游戏是免费共享游戏,使用的游戏资源大部分取自\r\n<QQ三国>游戏.另:请勿使用本游戏用于其他用途\r\n仅供娱乐与讨论!"
		"\r\n\r\n*.本游戏与<QQ三国>仅玩法相似,内部数据和玩法实现\r\n均完全不同无法代入<QQ三国>相关数据!请玩家注意."
		"\r\n\r\n*.如游戏中遇到输入法无法切换,请切换窗口切换输入\r\n法后在切换游戏窗口"
		"\r\n\r\n*.如遇到游戏资源加载失败,请使用兼容模式并且管理\r\n员运行."
		"\r\n\r\n*.本游戏只是游戏爱好者追忆的娱乐之作,请勿与\r\n<QQ三国>混淆.";
	RECT lprect = { (LONG)TipWidth / 2 +10,(LONG)TipTop + 40,CL_WinWidth,CL_WinHeight };
	D2Dx9.DrawTextFont(TipText, 20, 10, FALSE, "隶书", &lprect, D3DCOLOR_XRGB(0, 150, 230));
	if (MousePoint.x > (int)TipWidth - 35 && MousePoint.x < (int)TipWidth + 17 && MousePoint.y < (int)TipHeight - 20 && MousePoint.y >(int)TipHeight - 50) {
		if (MousePoint.KeyCode == MK_LBUTTON) { D2Dx9.DrawTexture(Submit[2], (float)TipWidth - 30, (float)TipHeight - 20); DrawTip = FALSE; DrawCreatPlyer = TRUE; }
		else D2Dx9.DrawTexture(Submit[0], (float)TipWidth - 30, (float)TipHeight - 20);
	}
	else {
		D2Dx9.DrawTexture(Submit[1], (float)TipWidth - 30, (float)TipHeight - 20);
	}
	TipBox->Release();
	for (int i = 0; i < 3; i++) {
		Submit[i]->Release();
	}
}
//初始化绘制创建玩家资源;
void InitDrawCreatePlayer() {
	char szPath[MAX_PATH];
	GetExePathA(szPath, sizeof(szPath));
	strcat(szPath, "ResMap.map");
	FILE *pFile = fopen(szPath, "rb");
	if(!pFile) { MessageBox(0, L"资源包加载失败!", NULL, MB_OK); ExitProcess(NULL); }
	ResouceDataFile Resdatfile;
	ResouceDataFile::ResMapOInfo *MpImginfo = Resdatfile.GetMapImageInfo(szPath);
	if (!MpImginfo)return;
	if (DrawImgInf.MaxInt > 0) { MessageBox(0, L"程序加载资源出现未知!", NULL, MB_OK); ExitProcess(NULL); }
	DrawImgInf.DrawMap = (PDrawMapInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(DrawMapInfo)*MpImginfo->MaxCount);
	DrawImgInf.MaxInt = MpImginfo->MaxCount;
	if(!DrawImgInf.DrawMap) { MessageBox(0, L"申请内存失败,内存不足!", NULL, MB_OK); ExitProcess(NULL); }
	for (UINT i = 0; i < MpImginfo->MaxCount; i++) {
		ReadResMapInfo mpinf = MpImginfo->Mapinfo[i];
		PDrawMapInfo DrawMp = &DrawImgInf.DrawMap[i];
		DrawMp->x = mpinf.x;
		DrawMp->y = mpinf.y;
		DrawMp->width = mpinf.Width;
		DrawMp->height = mpinf.Height;
		DrawMp->Scale = mpinf.Scale;
		DrawMp->ImgLoadType = mpinf.ImgLoadType;
		DrawMp->rotation = 0.f;
		DrawMp->AnimateMaxCount = mpinf.AnimateCount;
		DrawMp->AnimateDelay = mpinf.AnimateDelay;
		DrawMp->AnimateOldTick = GetTickCount();
		DrawMp->AnimateTickIndex = 0;
		if (mpinf.ImgLoadType == Image) {
			Bytef *pUnzipAlloc = (Bytef*)LocalAlloc(LMEM_ZEROINIT, mpinf.ImageOriginSize);
			if (!pUnzipAlloc) { MessageBox(0, L"申请内存失败,内存不足!", NULL, MB_OK); ExitProcess(NULL); }
			Bytef *pDataAlloc = (Bytef*)LocalAlloc(LMEM_ZEROINIT, mpinf.ImageDataSize);
			if (!pDataAlloc) { MessageBox(0, L"申请内存失败,内存不足!", NULL, MB_OK); ExitProcess(NULL); }
			fseek(pFile, mpinf.ImageOffset, SEEK_SET);
			fread(pDataAlloc, 1, mpinf.ImageDataSize, pFile);
			UINT UnzipLen = mpinf.ImageOriginSize;
			uncompress(pUnzipAlloc, (uLongf*)&UnzipLen, pDataAlloc, mpinf.ImageDataSize);
			DrawMp->Texture = D2Dx9.LoadMemTexture(pUnzipAlloc, UnzipLen);
			DrawMp->ResAlloc = pUnzipAlloc;
			LocalFree(pDataAlloc);
		}
		else if (mpinf.ImgLoadType == Animate) {
			DrawMp->Animate = (PDrawMapInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(DrawMapInfo)*mpinf.AnimateCount);
			if (!DrawMp->Animate) { MessageBox(0, L"申请内存失败,内存不足!", NULL, MB_OK); ExitProcess(NULL); }
			for (UINT n = 0; n < mpinf.AnimateCount; n++) {
				ReadResMapInfo Anminf = mpinf.Animate[n];
				PDrawMapInfo DrawMpAnimate = DrawMp->Animate;
				Bytef *pUnzipAlloc = (Bytef*)LocalAlloc(LMEM_ZEROINIT, Anminf.ImageOriginSize);
				if (!pUnzipAlloc) { MessageBox(0, L"申请内存失败,内存不足!", NULL, MB_OK); ExitProcess(NULL); }
				Bytef *pDataAlloc = (Bytef*)LocalAlloc(LMEM_ZEROINIT, Anminf.ImageDataSize);
				if (!pDataAlloc) { MessageBox(0, L"申请内存失败,内存不足!", NULL, MB_OK); ExitProcess(NULL); }
				fseek(pFile, Anminf.ImageOffset, SEEK_SET);
				fread(pDataAlloc, 1, Anminf.ImageDataSize, pFile);
				UINT UnzipLen = Anminf.ImageOriginSize;
				uncompress(pUnzipAlloc, (uLongf*)&UnzipLen, pDataAlloc, Anminf.ImageDataSize);
				/*char fgg[MAX_PATH];
				GetExePathA(fgg, MAX_PATH);
				char obt[50];
				sprintf(obt, "%d_%d.bmp", i, n);
				strcat(fgg, obt);
				FILE *fil = fopen(fgg, "wb");
				fwrite(pUnzipAlloc, UnzipLen, 1, fil);
				fflush(fil);
				fclose(fil);*/
				DrawMpAnimate[n].Texture = D2Dx9.LoadMemTexture(pUnzipAlloc, Anminf.ImageOriginSize);
				DrawMpAnimate[n].ResAlloc = pUnzipAlloc;
				DrawMpAnimate[n].x = Anminf.x;
				DrawMpAnimate[n].y = Anminf.y;
				DrawMpAnimate[n].width = Anminf.Width;
				DrawMpAnimate[n].height = Anminf.Height;
				DrawMpAnimate[n].Scale = Anminf.Scale;
				DrawMpAnimate[n].ImgLoadType = Anminf.ImgLoadType;
				DrawMpAnimate[n].rotation = 0.f;
				DrawMpAnimate[n].AnimateMaxCount = Anminf.AnimateCount;
				DrawMpAnimate[n].AnimateDelay = Anminf.AnimateDelay;
				LocalFree(pDataAlloc);
			}
		}
	}
	fclose(pFile);
}
//绘制玩家角色创建;
void DrawCreatePlayer() {
	for (UINT i = 0; i < DrawImgInf.MaxInt; i++) {
		PDrawMapInfo DrwMp = &DrawImgInf.DrawMap[i];
		if (DrwMp->ImgLoadType == Image) {
			D2Dx9.DrawTexture(DrwMp->Texture, DrwMp->x, DrwMp->y, DrwMp->width, DrwMp->height, DrwMp->Scale, DrwMp->rotation);
		}
		else if (DrwMp->ImgLoadType == Animate) {
			if (GetTickCount() - DrwMp->AnimateOldTick > DrwMp->AnimateDelay) {
				DrwMp->AnimateTickIndex++;
				DrwMp->AnimateOldTick = GetTickCount();
				if (DrwMp->AnimateTickIndex >= DrwMp->AnimateMaxCount)
					DrwMp->AnimateTickIndex = 0;
			}
			DrawMapInfo DrwMpAn = DrwMp->Animate[DrwMp->AnimateTickIndex];
			D2Dx9.DrawTexture(DrwMpAn.Texture, DrwMpAn.x, DrwMpAn.y, DrwMpAn.width, DrwMpAn.height, DrwMpAn.Scale, DrwMpAn.rotation);
		}
	}
}
