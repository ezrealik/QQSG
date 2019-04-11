#include "Main.h"
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	WNDCLASS WinClass = { 0 };
	WinClass.hInstance = hInstance;
	WinClass.lpszClassName = GVar_ClassName;
	WinClass.lpfnWndProc = WndProc;
	WinClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WinClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WinClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	WinClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	int WinX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (CL_WinWidth / 2),
		WinY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (CL_WinHeight / 2);
	if (!RegisterClass(&WinClass))MessageBox(0, L"注册窗口类名失败!", NULL, MB_OK);
	GVar_hWnd = CreateWindow(GVar_ClassName, L"萌萌三国单机版-Ver1.21",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		WinX, WinY, CL_WinWidth, CL_WinHeight, NULL, NULL, hInstance, NULL);
	if (!GVar_hWnd)MsgTipExit("窗口创建失败!");
	ShowWindow(GVar_hWnd, SW_SHOWNORMAL);
	UpdateWindow(GVar_hWnd);
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
		CoInitialize(nullptr);
		GVar_Ime.SetMaxLen(14);
		//初始化程序;
		RECT ClRect;
		GetClientRect(hWnd, &ClRect);
		if (!GVar_D2Dx9.InitD3D(hWnd, ClRect.right - ClRect.left, ClRect.bottom - ClRect.top, false)) MsgTipExit("初始化D3D失败!");
		if (!BASS_Init(-1, 44100, BASS_DEVICE_3D, hWnd, NULL)) MsgTipExit("初始化BASS音频库失败!");
		LPDIRECT3DDEVICE9 D3DDevice9 = GVar_D2Dx9.GetD3Devicex9();
		D3DDevice9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		D3DDevice9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		D3DDevice9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		D3DDevice9->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);
		//播放BGM;
		PlayerBGM_Login("Login1.mp3");
		GVar_MousePoint.OldTick = GetTickCount();
		GVar_ImeTick = GetTickCount();
		DrawTipDialogBox();
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)D3DGameRun, NULL, NULL, NULL);
		break;
	}
	case WM_CLOSE:
		BASS_Free();
		ExitProcess(0);
		break;
	case WM_KEYDOWN:
		GVar_Ime.SetKeyState(TRUE);
		ImeEvent(wParam);
		switch (LOWORD(wParam))
		{
		case VK_LEFT:
			if (GVar_PlayerImageInfo.MaxInt >= 1)GVar_PlayerImageInfo.AnimateState = 5;
			GVar_CreenCentreX -= 2;
			break;
		case VK_RIGHT:
			if (GVar_PlayerImageInfo.MaxInt >= 1)GVar_PlayerImageInfo.AnimateState = 4;
			GVar_CreenCentreX += 2;
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		GVar_Ime.SetKeyState(FALSE);
		switch (LOWORD(wParam))
		{
		case VK_LEFT:
			GVar_PlayerImageInfo.AnimateState = 1;
			break;
		case VK_RIGHT:
			GVar_PlayerImageInfo.AnimateState = 0;
			break;
		default:
			break;
		}
		break;
	case WM_MOUSEMOVE:
		GVar_MousePoint.KeyCode = wParam;
		GVar_MousePoint.x = GET_X_LPARAM(lParam);
		GVar_MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONDOWN:
		GVar_MousePoint.KeyCode = wParam;
		GVar_MousePoint.KeyState = WM_LBUTTONDOWN;
		GVar_MousePoint.x = GET_X_LPARAM(lParam);
		GVar_MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_RBUTTONDOWN:
		GVar_MousePoint.KeyCode = wParam;
		GVar_MousePoint.KeyState = WM_RBUTTONDOWN;
		GVar_MousePoint.x = GET_X_LPARAM(lParam);
		GVar_MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONUP:
		GVar_MousePoint.KeyCode = wParam;
		GVar_MousePoint.KeyState = WM_LBUTTONUP;
		GVar_MousePoint.x = GET_X_LPARAM(lParam);
		GVar_MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_RBUTTONUP:
		GVar_MousePoint.KeyCode = wParam;
		GVar_MousePoint.KeyState = WM_RBUTTONUP;
		GVar_MousePoint.x = GET_X_LPARAM(lParam);
		GVar_MousePoint.y = GET_Y_LPARAM(lParam);
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
		GVar_Ime.DeleteImeStr();
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
		GVar_Ime.SetImeChar(chc);
	}
}
//资源文件加载;
UINT LoadResourceData(const char *pFile, const char *pResName, void **pAlloc){
	ResouceDataFile ResData;
	ResouceDataFile::ResDataIndexInfo *ResFa = nullptr;
	C_Module Mod;
	const char *path = Mod.GetCurrencyPathFileA(pFile);
	ResFa = ResData.GetResDataIndex(path);
	UINT RNumber = ResData.GetResIndexNumber();
	void *Result = nullptr;
	uLongf UnzipLen = 0;
	for (UINT i = 0; i < RNumber; i++) {
		//如果是登录1背景音乐,则播放;
		char *pFileName = (char*)ResFa[i].pFileName;
		if (!pFileName)continue;
		if (strcmp((char*)ResFa[i].pFileName, pResName) == 0) {
			//申请内存音频流缓冲区
			Result = LocalAlloc(LMEM_ZEROINIT, ResFa[i].pOriginSize);
			if (!Result) MsgTipExit("申请内存失败,内存不足!");
			//申请临时解压内存音频流缓冲区;
			void *TmpAlloc = LocalAlloc(LMEM_ZEROINIT, ResFa[i].pDataSize);
			if (!TmpAlloc) MsgTipExit("申请内存失败,内存不足!");
			//读取音频流数据存入内存
			FILE *pFile = fopen(path, "rb");
			if (!pFile)MsgTipExit("资源打开失败,请确认文件是否损坏!");
			fseek(pFile, ResFa[i].pOffset, SEEK_SET);
			fread(TmpAlloc, ResFa[i].pDataSize, 1, pFile);
			UnzipLen = ResFa[i].pOriginSize;
			//解压音频数据流;
			uncompress((Bytef*)Result, &UnzipLen, (Bytef*)TmpAlloc, ResFa[i].pDataSize);
			//释放申请的资源;
			LocalFree(TmpAlloc);
			fclose(pFile);
			break;
		}
	}
	*pAlloc = Result;
	return UnzipLen;
}
//释放加载的资源文件;
void ReleaseResourceData(void *pAlloc) {
	LocalFree(pAlloc);
}
//播放登录背景音乐
void PlayerBGM_Login(const char *MusicName) {
	UINT UnzipLen = LoadResourceData("Music.db", MusicName, &GVar_BGM_Login1);
	GVar_StreamBGM1 = BASS_StreamCreateFile(TRUE, GVar_BGM_Login1, 0, UnzipLen, BASS_SAMPLE_LOOP);
	if (!GVar_StreamBGM1)MsgTipExit("音频打开失败,请确认资源文件是否损坏!");
	//设置背景音乐音量;
	BASS_ChannelSetAttribute(GVar_StreamBGM1, BASS_ATTRIB_VOL, 100);
	//播放音频;
	BASS_ChannelPlay(GVar_StreamBGM1, TRUE);
}
//游戏绘制
void D3DGameRun() {
	while (GVar_DrawGame)
	{
		LPDIRECT3DDEVICE9 D3DDevice9 = GVar_D2Dx9.GetD3Devicex9();
		if (!D3DDevice9)return;
		GVar_D2Dx9.Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		if (SUCCEEDED(D3DDevice9->BeginScene())) {
			//绘制提示框;
			//if (DrawTip)DrawTipBox();
			if (GVar_DrawCreatPlyer)DrawCreatePlayer();
			if (GVar_IsTipBox)DrawTipDialogBox();
			if (GVar_DrawCityMappl)DrawCityMap();
			ShowFPS();
			D3DDevice9->EndScene();
			D3DDevice9->Present(NULL, NULL, NULL, NULL);
		}
		Sleep(1);
	}

}
//提示对话框;
void SetTipDialogBoxText(const char *pText) {
	strcpy(GVar_szTipBoxText, pText);
	GVar_IsTipBox = TRUE;
}
void InitTipDialogBox() {
	GVar_ClTipButton.MaxButton = 2;
	UINT MaxAllocLen = sizeof(ButtonImageInfo) * GVar_ClTipButton.MaxButton;
	GVar_ClTipButton.Button = (PButtonImageInfo)LocalAlloc(LMEM_ZEROINIT, MaxAllocLen);
	PButtonImageInfo pButon2 = &GVar_ClTipButton.Button[0];
	pButon2->ButtonNum = 1;
	pButon2->ButtonType = Button_Dan;
	void *pAl;
	UINT pAllen = LoadResourceData("UIRes\\CreatePlayer\\UIButton.db", "UIRes\\CreatePlayer\\19275-1.bmp", &pAl);
	if (pAllen < 0xFF)MsgTipExit("加载资源失败,请确认资源包是否损坏!");
	pButon2->SelectIndex = 0;
	D3DXIMAGE_INFO info = GVar_D2Dx9.GetImageInfoInMemory(pAl, pAllen);
	pButon2->Width = info.Width;
	pButon2->Height = info.Height;
	pButon2->x = (float)(CL_WinWidth / 2) - (info.Width / 2) - 10;
	pButon2->y = (float)(CL_WinHeight / 2) - (info.Height / 2);
	pButon2->Texture = GVar_D2Dx9.LoadMemTexture(pAl, pAllen);
	pButon2->ResAlloc = pAl;
	//确认按钮
	char szFile[3][50] = { "UIRes\\CreatePlayer\\63717-2.bmp","UIRes\\CreatePlayer\\63717-3.bmp" ,"UIRes\\CreatePlayer\\63717-1.bmp" };
	PButtonImageInfo pButon = &GVar_ClTipButton.Button[1];
	pButon->ButtonType = Button_Duo;
	pButon->ButtonNum = 3;
	pButon->MultiButton = (PButtonImageInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(ButtonImageInfo) * pButon->ButtonNum);
	if (!pButon->MultiButton)MsgTipExit("申请内存失败,内存不足!");
	pButon->SelectIndex = 0;
	for (UINT i = 0; i < pButon->ButtonNum; i++) {
		pButon->MultiButton[i].x = (float)(CL_WinWidth / 2) - 40;
		pButon->MultiButton[i].y = (float)CL_WinHeight / 2 + 30;
		void *pAl;
		UINT pAllen = LoadResourceData("UIRes\\CreatePlayer\\UIButton.db", szFile[i], &pAl);
		if (pAllen < 0xFF)MsgTipExit("加载资源失败,请确认资源包是否损坏!");
		D3DXIMAGE_INFO info = GVar_D2Dx9.GetImageInfoInMemory(pAl, pAllen);
		pButon->MultiButton[i].Width = info.Width;
		pButon->MultiButton[i].Height = info.Height;
		pButon->MultiButton[i].Texture = GVar_D2Dx9.LoadMemTexture(pAl, pAllen);
		pButon->MultiButton[i].ResAlloc = pAl;
	}
	GVar_ClTipButton.Button[0].SelectIndex = 0;
}
void DrawTipDialogBox() {
	if (GVar_ClTipButton.MaxButton < 1)InitTipDialogBox();
	for (UINT i = 0; i < GVar_ClTipButton.MaxButton; i++) {
		if (GVar_ClTipButton.Button[i].ButtonType == Button_Dan) {
			GVar_D2Dx9.DrawTexture(GVar_ClTipButton.Button[i].Texture, GVar_ClTipButton.Button[i].x, GVar_ClTipButton.Button[i].y, GVar_ClTipButton.Button[i].Width, GVar_ClTipButton.Button[i].Height);
		}
		else if (GVar_ClTipButton.Button[i].ButtonType == Button_Duo) {
			ButtonImageInfo &pButon = GVar_ClTipButton.Button[i].MultiButton[GVar_ClTipButton.Button[i].SelectIndex];
			if (IsRectMouse(pButon.x, pButon.y, pButon.Width, pButon.Height, GVar_MousePoint)) {
				if (GVar_MousePoint.KeyState == WM_LBUTTONDOWN) {
					GVar_ClTipButton.Button[i].SelectIndex = 2;
					if (ClickMouseDelay())GVar_IsTipBox = FALSE;
				}
				else GVar_ClTipButton.Button[i].SelectIndex = 1;
			}
			else GVar_ClTipButton.Button[i].SelectIndex = 0;
			GVar_D2Dx9.DrawTexture(pButon.Texture, pButon.x, pButon.y, pButon.Width, pButon.Height);
		}
	}
	RECT lRfont = { (LONG)GVar_ClTipButton.Button[0].x + 20, (LONG)GVar_ClTipButton.Button[0].y + 20,CL_WinWidth,CL_WinHeight };
	GVar_D2Dx9.DrawTextFont(GVar_szTipBoxText, 15, 1, FALSE, "黑体", &lRfont, D3DCOLOR_XRGB(255, 235, 147));
}
//显示FPS帧率:
void ShowFPS() {
	//计算每秒FPS帧数;
	if (GVar_OldFpsTick == 0 || GetTickCount() - GVar_OldFpsTick > 1000) {
		GVar_OldFpsTick = GetTickCount();
		GVar_NewFPSCount = GVar_FPSCount;
		GVar_FPSCount = 0;
	}
	else GVar_FPSCount++;
	char FPSOut[50];
	sprintf(FPSOut, "FPS:%d", GVar_NewFPSCount);
	RECT lrect = { CL_WinWidth - 80,10,CL_WinWidth,50 };
	GVar_D2Dx9.DrawFont(FPSOut, 12, NULL, FALSE, "隶书", &lrect, D3DCOLOR_XRGB(0, 250, 0));
}
//绘制提示框;
void DrawTipBox() {
	UINT TipWidth = 500, TipHeight = 600, TipTop = 30;
	WCHAR szPath[MAX_PATH];
	C_Module Mod;
	WCHAR szImg_Tip[MAX_PATH];
	wcscpy(szImg_Tip, Mod.GetExePathW());
	wcscat(szImg_Tip, L"UIRes\\登录UI\\登录角色\\提示框-1.bmp");
	LPDIRECT3DTEXTURE9 TipBox = GVar_D2Dx9.LoadRectTexture(szImg_Tip, TipWidth, TipHeight);
	WCHAR SubmitT[3][256] = { L"UIRes\\登录UI\\登录角色\\确定-1.bmp",L"UIRes\\登录UI\\登录角色\\确定-2.bmp",L"UIRes\\登录UI\\登录角色\\确定-3.bmp" };
	LPDIRECT3DTEXTURE9 Submit[3] = { 0 };
	for (int i = 0; i < 3; i++) {
		wcscpy(szImg_Tip, szPath);
		wcscat(szImg_Tip, SubmitT[i]);
		Submit[i] = GVar_D2Dx9.LoadTexture(szImg_Tip);
	}
	GVar_D2Dx9.DrawTexture(TipBox, (float)TipWidth / 2, (float)TipTop);
	char TipT[] = "关于游戏:";
	RECT lrect = { (LONG)TipWidth - 70,(LONG)TipTop + 5,CL_WinWidth,CL_WinHeight };
	GVar_D2Dx9.DrawFont(TipT, 25, 10, TRUE, "黑体", &lrect, D3DCOLOR_XRGB(255, 0, 0));
	char TipText[] = "*.本游戏是免费共享游戏,使用的游戏资源大部分取自\r\n<QQ三国>游戏.另:请勿使用本游戏用于其他用途\r\n仅供娱乐与讨论!"
		"\r\n\r\n*.本游戏与<QQ三国>仅玩法相似,内部数据和玩法实现\r\n均完全不同无法代入<QQ三国>相关数据!请玩家注意."
		"\r\n\r\n*.如游戏中遇到输入法无法切换,请切换窗口切换输入\r\n法后在切换游戏窗口"
		"\r\n\r\n*.如遇到游戏资源加载失败,请使用兼容模式并且管理\r\n员运行."
		"\r\n\r\n*.本游戏只是游戏爱好者追忆的娱乐之作,请勿与\r\n<QQ三国>混淆.";
	RECT lprect = { (LONG)TipWidth / 2 +10,(LONG)TipTop + 40,CL_WinWidth,CL_WinHeight };
	GVar_D2Dx9.DrawTextFont(TipText, 20, 10, FALSE, "隶书", &lprect, D3DCOLOR_XRGB(0, 150, 230));
	if (GVar_MousePoint.x > (int)TipWidth - 35 && GVar_MousePoint.x < (int)TipWidth + 17 && GVar_MousePoint.y < (int)TipHeight - 20 && GVar_MousePoint.y >(int)TipHeight - 50) {
		if (GVar_MousePoint.KeyCode == MK_LBUTTON) { GVar_D2Dx9.DrawTexture(Submit[2], (float)TipWidth - 30, (float)TipHeight - 20); GVar_DrawTip = FALSE; GVar_DrawCreatPlyer = TRUE; }
		else GVar_D2Dx9.DrawTexture(Submit[0], (float)TipWidth - 30, (float)TipHeight - 20);
	}
	else {
		GVar_D2Dx9.DrawTexture(Submit[1], (float)TipWidth - 30, (float)TipHeight - 20);
	}
	TipBox->Release();
	for (int i = 0; i < 3; i++) {
		Submit[i]->Release();
	}
}
//地图资源加载;
void LoadMapResourceData(ResouceDataFile::ResMapOInfo *ResMpIOinfo, DrawImageInfo &pMapAlloc, UINT pLen, const char*pMapFile) {
	FILE *pFile = fopen(pMapFile, "rb");
	if (!pFile)MsgTipExit("程序加载资源出现未知错误!");
	for (UINT i = 0; i < ResMpIOinfo->MaxCount; i++) {
		ReadResMapInfo mpinf = ResMpIOinfo->Mapinfo[i];
		PDrawMapInfo DrawMp = &pMapAlloc.DrawMap[i];
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
		if (mpinf.ImgLoadType == _Image) {
			Bytef *pUnzipAlloc = (Bytef*)LocalAlloc(LMEM_ZEROINIT, mpinf.ImageOriginSize);
			if (!pUnzipAlloc) MsgTipExit("程序加载资源出现未知错误!");
			Bytef *pDataAlloc = (Bytef*)LocalAlloc(LMEM_ZEROINIT, mpinf.ImageDataSize);
			if (!pDataAlloc) MsgTipExit("程序加载资源出现未知错误!");
			fseek(pFile, mpinf.ImageOffset, SEEK_SET);
			fread(pDataAlloc, 1, mpinf.ImageDataSize, pFile);
			UINT UnzipLen = mpinf.ImageOriginSize;
			uncompress(pUnzipAlloc, (uLongf*)&UnzipLen, pDataAlloc, mpinf.ImageDataSize);
			DrawMp->Texture = GVar_D2Dx9.LoadMemTexture(pUnzipAlloc, UnzipLen);
			DrawMp->ResAlloc = pUnzipAlloc;
			LocalFree(pDataAlloc);
		}
		else if (mpinf.ImgLoadType == Animate) {
			DrawMp->Animate = (PDrawMapInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(DrawMapInfo)*mpinf.AnimateCount);
			if (!DrawMp->Animate)MsgTipExit("程序加载资源出现未知错误!");
			for (UINT n = 0; n < mpinf.AnimateCount; n++) {
				ReadResMapInfo Anminf = mpinf.Animate[n];
				PDrawMapInfo DrawMpAnimate = DrawMp->Animate;
				Bytef *pUnzipAlloc = (Bytef*)LocalAlloc(LMEM_ZEROINIT, Anminf.ImageOriginSize);
				if (!pUnzipAlloc)MsgTipExit("程序加载资源出现未知错误!");
				Bytef *pDataAlloc = (Bytef*)LocalAlloc(LMEM_ZEROINIT, Anminf.ImageDataSize);
				if (!pDataAlloc)MsgTipExit("程序加载资源出现未知错误!");
				fseek(pFile, Anminf.ImageOffset, SEEK_SET);
				fread(pDataAlloc, 1, Anminf.ImageDataSize, pFile);
				UINT UnzipLen = Anminf.ImageOriginSize;
				uncompress(pUnzipAlloc, (uLongf*)&UnzipLen, pDataAlloc, Anminf.ImageDataSize);
				if (mpinf.Scale == 0x86733FA) {
					C_Module Mod;
					Mod.MirrorBitmap(pUnzipAlloc, Anminf.ImageOriginSize);
				}
				DrawMpAnimate[n].Texture = GVar_D2Dx9.LoadMemTexture(pUnzipAlloc, Anminf.ImageOriginSize);
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
//释放已加载的地图资源
void ReleaseMapResource(DrawImageInfo &PMapImageInfo) {
	if (PMapImageInfo.MaxInt < 1 || PMapImageInfo.DrawMap == nullptr)return;
	for (UINT i = 0; i < PMapImageInfo.MaxInt; i++) {
		if (PMapImageInfo.DrawMap[i].ImgLoadType == _Image) {
			if (PMapImageInfo.DrawMap[i].Texture)PMapImageInfo.DrawMap[i].Texture->Release();
			LocalFree(PMapImageInfo.DrawMap[i].ResAlloc);
		}
		else if (PMapImageInfo.DrawMap[i].ImgLoadType == Animate) {
			for (UINT n = 0; n < PMapImageInfo.DrawMap[i].AnimateMaxCount; n++) {
				if (PMapImageInfo.DrawMap[i].Animate[n].Texture)PMapImageInfo.DrawMap[i].Animate[n].Texture->Release();
				LocalFree(PMapImageInfo.DrawMap[i].Animate[n].ResAlloc);
			}
		}
	}
	LocalFree(PMapImageInfo.DrawMap);
	PMapImageInfo = { 0 };
}
//释放已加载的按钮资源
void ReleaseButtonResource(ButtonInfo &PMapImageInfo){
	if (PMapImageInfo.MaxButton < 1 || PMapImageInfo.Button == nullptr)return;
	for (UINT i = 0; i < PMapImageInfo.MaxButton; i++) {
		if (PMapImageInfo.Button[i].ButtonType == Button_Dan) {
			if (PMapImageInfo.Button[i].Texture)PMapImageInfo.Button[i].Texture->Release();
			LocalFree(PMapImageInfo.Button[i].ResAlloc);
		}
		else if (PMapImageInfo.Button[i].ButtonType == Button_Duo) {
			for (UINT n = 0; n < PMapImageInfo.Button[i].ButtonNum; n++) {
				if (PMapImageInfo.Button[i].MultiButton[n].Texture)PMapImageInfo.Button[i].MultiButton[n].Texture->Release();
				LocalFree(PMapImageInfo.Button[i].MultiButton[n].ResAlloc);
			}
		}
	}
	LocalFree(PMapImageInfo.Button);
	PMapImageInfo = { 0 };
}
//判断鼠标是否在选区内
BOOL IsRectMouse(float x, float y, UINT Width, UINT Height, MouseInfo const&MousePoint) {
	float mX = 0.f, mY = 0.f;
	if (MousePoint.x - mX > x && MousePoint.x - mX < x + (float)Width && MousePoint.y - mY > y && MousePoint.y - mY < y + (float)Height) {
		return TRUE;
	}
	return FALSE;
}
//初始化绘制创建玩家资源;
void InitDrawCreatePlayer() {
	C_Module Mod;
	const char *szPath = Mod.GetCurrencyPathFileA("创建角色背景.map");
	ResouceDataFile Resdatfile;
	ResouceDataFile::ResMapOInfo *MpImginfo = Resdatfile.GetMapImageInfo(szPath);
	if (!MpImginfo) MsgTipExit("程序加载资源出现未知错误!");
	if (GVar_MapImageInfo.MaxInt > 0)MsgTipExit("程序加载资源出现未知错误!");
	UINT MaxAllocLen = sizeof(DrawMapInfo)*MpImginfo->MaxCount;
	GVar_MapImageInfo.DrawMap = (PDrawMapInfo)LocalAlloc(LMEM_ZEROINIT, MaxAllocLen);
	GVar_MapImageInfo.MaxInt = MpImginfo->MaxCount;
	if(!GVar_MapImageInfo.DrawMap) MsgTipExit("申请内存失败,内存不足!");
	LoadMapResourceData(MpImginfo, GVar_MapImageInfo, MaxAllocLen, szPath);
	//加载人物动画;
	Resdatfile.Release();
	PlayerChange("UIRes\\CreatePlayer\\nv-yy.map", GVar_PlayerImageInfo);
	//加载按钮;
	GVar_ClButton.MaxButton = 11;
	MaxAllocLen = sizeof(ButtonImageInfo) * GVar_ClButton.MaxButton;
	GVar_ClButton.Button = (PButtonImageInfo)LocalAlloc(LMEM_ZEROINIT, MaxAllocLen);
	if (!GVar_ClButton.Button) MsgTipExit("申请内存失败,内存不足!");
#pragma region 按钮初始化;
	//职业选择按钮;
	char szFile[5][2][50] = { "UIRes\\CreatePlayer\\阴阳士-1.bmp","UIRes\\CreatePlayer\\阴阳士-2.bmp",
		"UIRes\\CreatePlayer\\剑侍-1.bmp","UIRes\\CreatePlayer\\剑侍-2.bmp","UIRes\\CreatePlayer\\豪杰-1.bmp","UIRes\\CreatePlayer\\豪杰-2.bmp",
		"UIRes\\CreatePlayer\\仙术士-1.bmp","UIRes\\CreatePlayer\\仙术士-2.bmp","UIRes\\CreatePlayer\\游侠-1.bmp","UIRes\\CreatePlayer\\游侠-2.bmp"
	};
	for (UINT t = 0; t < 5; t++) {
		PButtonImageInfo pButon = &GVar_ClButton.Button[t];
		pButon->ButtonType = Button_Duo;
		pButon->ButtonNum = 2;
		pButon->MultiButton = (PButtonImageInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(ButtonImageInfo) * pButon->ButtonNum);
		if (!pButon->MultiButton)MsgTipExit("申请内存失败,内存不足!");
		pButon->SelectIndex = 0;
		for (UINT i = 0; i < pButon->ButtonNum; i++) {
			pButon->MultiButton[i].x = (float)(CL_WinWidth / 2) - 200 + (t * 80);
			pButon->MultiButton[i].y = (float)CL_WinHeight - 150;
			void *pAl;
			UINT pAllen = LoadResourceData("UIRes\\CreatePlayer\\UIButton.db", szFile[t][i], &pAl);
			if (pAllen < 0xFF)MsgTipExit("加载资源失败,请确认资源包是否损坏!");
			D3DXIMAGE_INFO info = GVar_D2Dx9.GetImageInfoInMemory(pAl, pAllen);
			pButon->MultiButton[i].Width = info.Width;
			pButon->MultiButton[i].Height = info.Height;
			pButon->MultiButton[i].Texture = GVar_D2Dx9.LoadMemTexture(pAl, pAllen);
			pButon->MultiButton[i].ResAlloc = pAl;
		}
	}
	GVar_ClButton.Button[0].SelectIndex = 1;
	//开始游戏按钮;
	char szStartGame[3][50] = { "UIRes\\CreatePlayer\\开始游戏-2.bmp","UIRes\\CreatePlayer\\开始游戏-3.bmp" ,"UIRes\\CreatePlayer\\开始游戏-1.bmp" };
	PButtonImageInfo pButon = &GVar_ClButton.Button[5];
	pButon->ButtonType = Button_Duo;
	pButon->ButtonNum = 3;
	pButon->MultiButton = (PButtonImageInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(ButtonImageInfo) * pButon->ButtonNum);
	if (!pButon->MultiButton)MsgTipExit("申请内存失败,内存不足!");
	pButon->SelectIndex = 0;
	for (UINT i = 0; i < pButon->ButtonNum; i++) {
		pButon->MultiButton[i].x = (float)(CL_WinWidth / 2) +100;
		pButon->MultiButton[i].y = (float)CL_WinHeight - 75;
		void *pAl;
		UINT pAllen = LoadResourceData("UIRes\\CreatePlayer\\UIButton.db", szStartGame[i], &pAl);
		if (pAllen < 0xFF)MsgTipExit("加载资源失败,请确认资源包是否损坏!");
		D3DXIMAGE_INFO info = GVar_D2Dx9.GetImageInfoInMemory(pAl, pAllen);
		pButon->MultiButton[i].Width = info.Width;
		pButon->MultiButton[i].Height = info.Height;
		pButon->MultiButton[i].Texture = GVar_D2Dx9.LoadMemTexture(pAl, pAllen);
		pButon->MultiButton[i].ResAlloc = pAl;
	}
	//国家选择按钮;
	char szGuoFile[3][2][50] = { "UIRes\\CreatePlayer\\魏-1.bmp","UIRes\\CreatePlayer\\魏-2.bmp",
		"UIRes\\CreatePlayer\\吴-1.bmp","UIRes\\CreatePlayer\\吴-2.bmp","UIRes\\CreatePlayer\\蜀-1.bmp","UIRes\\CreatePlayer\\蜀-2.bmp",
	};
	for (UINT t = 6; t < 9; t++) {
		PButtonImageInfo pButon = &GVar_ClButton.Button[t];
		pButon->ButtonType = Button_Duo;
		pButon->ButtonNum = 2;
		pButon->MultiButton = (PButtonImageInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(ButtonImageInfo) * pButon->ButtonNum);
		if (!pButon->MultiButton)MsgTipExit("申请内存失败,内存不足!");
		pButon->SelectIndex = 0;
		for (UINT i = 0; i < pButon->ButtonNum; i++) {
			pButon->MultiButton[i].x = (float)CL_WinWidth - 180 + ((t - 6) * 45);
			pButon->MultiButton[i].y = (float)35;
			void *pAl;
			UINT pAllen = LoadResourceData("UIRes\\CreatePlayer\\UIButton.db", szGuoFile[t - 6][i], &pAl);
			if (pAllen < 0xFF)MsgTipExit("加载资源失败,请确认资源包是否损坏!");
			D3DXIMAGE_INFO info = GVar_D2Dx9.GetImageInfoInMemory(pAl, pAllen);
			pButon->MultiButton[i].Width = info.Width;
			pButon->MultiButton[i].Height = info.Height;
			pButon->MultiButton[i].Texture = GVar_D2Dx9.LoadMemTexture(pAl, pAllen);
			pButon->MultiButton[i].ResAlloc = pAl;
		}
	}
	GVar_ClButton.Button[6].SelectIndex = 1;
	//性别选择按钮;
	char szWmFile[2][2][50] = { "UIRes\\CreatePlayer\\男-1.bmp","UIRes\\CreatePlayer\\男-2.bmp",
		"UIRes\\CreatePlayer\\女-1.bmp","UIRes\\CreatePlayer\\女-2.bmp"};
	for (UINT t = 9; t < 11; t++) {
		PButtonImageInfo pButon = &GVar_ClButton.Button[t];
		pButon->ButtonType = Button_Duo;
		pButon->ButtonNum = 2;
		pButon->MultiButton = (PButtonImageInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(ButtonImageInfo) * pButon->ButtonNum);
		if (!pButon->MultiButton)MsgTipExit("申请内存失败,内存不足!");
		pButon->SelectIndex = 0;
		for (UINT i = 0; i < pButon->ButtonNum; i++) {
			pButon->MultiButton[i].x = (float)CL_WinWidth - 180 + ((t - 9) * 80);
			pButon->MultiButton[i].y = (float)90;
			void *pAl;
			UINT pAllen = LoadResourceData("UIRes\\CreatePlayer\\UIButton.db", szWmFile[t - 9][i], &pAl);
			if (pAllen < 0xFF)MsgTipExit("加载资源失败,请确认资源包是否损坏!");
			D3DXIMAGE_INFO info = GVar_D2Dx9.GetImageInfoInMemory(pAl, pAllen);
			pButon->MultiButton[i].Width = info.Width;
			pButon->MultiButton[i].Height = info.Height;
			pButon->MultiButton[i].Texture = GVar_D2Dx9.LoadMemTexture(pAl, pAllen);
			pButon->MultiButton[i].ResAlloc = pAl;
		}
	}
	GVar_ClButton.Button[10].SelectIndex = 1;
#pragma endregion

}
//绘制播放职业玩家;
void PlayerChange(const char *pMapFile, DrawImageInfo &pMapAlloc) {
	ReleaseMapResource(pMapAlloc);
	//加载人物动画;
	C_Module Mod;
	ResouceDataFile Resdatfile;
	ResouceDataFile::ResMapOInfo *MpImginfo = nullptr;
	const char *szPath = Mod.GetCurrencyPathFileA(pMapFile);
	MpImginfo = Resdatfile.GetMapImageInfo(szPath);
	if (!MpImginfo) MsgTipExit("程序加载资源出现未知错误!");
	if (pMapAlloc.MaxInt > 0) MsgTipExit("程序加载资源出现未知错误!");
	UINT MaxAllocLen = sizeof(DrawMapInfo) * MpImginfo->MaxCount;
	pMapAlloc.DrawMap = (PDrawMapInfo)LocalAlloc(LMEM_ZEROINIT, MaxAllocLen);
	pMapAlloc.MaxInt = MpImginfo->MaxCount;
	LoadMapResourceData(MpImginfo, pMapAlloc, MaxAllocLen, szPath);
}
//鼠标点击延迟计算;
BOOL ClickMouseDelay() {
	if (GetTickCount() - GVar_MousePoint.OldTick > 300) {
		GVar_MousePoint.OldTick = GetTickCount();
		return TRUE;
	}
	return FALSE;
}
void PlayerButton() {
	if (ClickMouseDelay()) {
		switch (GVar_玩家职业)
		{
		case 0:
			if (!GVar_玩家性别是否为男)PlayerChange("UIRes\\CreatePlayer\\nv-yy.map", GVar_PlayerImageInfo);
			else PlayerChange("UIRes\\CreatePlayer\\nan-yy.map", GVar_PlayerImageInfo);
			strcpy(GVar_szCareerTip, "        阴阳士\r\n\r\n远程丶术攻丶高攻丶施法慢");
			break;
		case 1:
			if (!GVar_玩家性别是否为男)PlayerChange("UIRes\\CreatePlayer\\nv-js.map", GVar_PlayerImageInfo);
			else PlayerChange("UIRes\\CreatePlayer\\nan-js.map", GVar_PlayerImageInfo);
			strcpy(GVar_szCareerTip, "        剑侍\r\n\r\n近战丶物理丶平均丶攻速快");
			break;
		case 2:
			if (!GVar_玩家性别是否为男)PlayerChange("UIRes\\CreatePlayer\\nv-hj.map", GVar_PlayerImageInfo);
			else PlayerChange("UIRes\\CreatePlayer\\nan-hj.map", GVar_PlayerImageInfo);
			strcpy(GVar_szCareerTip, "        豪杰\r\n\r\n近战丶物理丶高防丶坦克肉");
			break;
		case 3:
			if (!GVar_玩家性别是否为男)PlayerChange("UIRes\\CreatePlayer\\nv-xs.map", GVar_PlayerImageInfo);
			else PlayerChange("UIRes\\CreatePlayer\\nan-xs.map", GVar_PlayerImageInfo);
			strcpy(GVar_szCareerTip, "        仙术士\r\n\r\n远程丶术攻丶治疗丶施法中");
			break;
		case 4:
			if (!GVar_玩家性别是否为男)PlayerChange("UIRes\\CreatePlayer\\nv-yx.map", GVar_PlayerImageInfo);
			else PlayerChange("UIRes\\CreatePlayer\\nan-yx.map", GVar_PlayerImageInfo);
			strcpy(GVar_szCareerTip, "        游侠\r\n\r\n远程丶物理丶高攻丶脆皮鸡");
			break;
		default:
			break;
		}
	}
}
//绘制玩家角色创建;
void DrawCreatePlayer() {
	//判断是否初始化过;
	if (GVar_MapImageInfo.DrawMap == nullptr && GVar_MapImageInfo.MaxInt < 1 && GVar_PlayerImageInfo.DrawMap == nullptr && GVar_PlayerImageInfo.MaxInt < 1)InitDrawCreatePlayer();
	//绘制显示UI;
	for (UINT i = 0; i < GVar_MapImageInfo.MaxInt; i++) {
		PDrawMapInfo DrwMp = &GVar_MapImageInfo.DrawMap[i];
		if (DrwMp->ImgLoadType == _Image) {
			GVar_D2Dx9.DrawTexture(DrwMp->Texture, DrwMp->x, DrwMp->y, DrwMp->width, DrwMp->height, DrwMp->Scale, DrwMp->rotation);
		}
		else if (DrwMp->ImgLoadType == Animate) {
			if (GetTickCount() - DrwMp->AnimateOldTick > DrwMp->AnimateDelay) {
				DrwMp->AnimateTickIndex++;
				DrwMp->AnimateOldTick = GetTickCount();
				if (DrwMp->AnimateTickIndex >= DrwMp->AnimateMaxCount)
					DrwMp->AnimateTickIndex = 0;
			}
			DrawMapInfo DrwMpAn = DrwMp->Animate[DrwMp->AnimateTickIndex];
			GVar_D2Dx9.DrawTexture(DrwMpAn.Texture, DrwMpAn.x, DrwMpAn.y, DrwMpAn.width, DrwMpAn.height, DrwMpAn.Scale, DrwMpAn.rotation);
		}
	}
	for (UINT i = 0; i < GVar_PlayerImageInfo.MaxInt; i++) {
		PDrawMapInfo DrwMp = &GVar_PlayerImageInfo.DrawMap[i];
		if (DrwMp->ImgLoadType == _Image) {
			GVar_D2Dx9.DrawTexture(DrwMp->Texture, DrwMp->x, DrwMp->y, DrwMp->width, DrwMp->height, DrwMp->Scale, DrwMp->rotation);
		}
		else if (DrwMp->ImgLoadType == Animate) {
			if (GetTickCount() - DrwMp->AnimateOldTick > DrwMp->AnimateDelay) {
				DrwMp->AnimateTickIndex++;
				DrwMp->AnimateOldTick = GetTickCount();
				if (DrwMp->AnimateTickIndex >= DrwMp->AnimateMaxCount) {
					DrwMp->AnimateTickIndex = 0;
				}
			}
			DrawMapInfo DrwMpAn = DrwMp->Animate[DrwMp->AnimateTickIndex];
			GVar_D2Dx9.DrawTexture(DrwMpAn.Texture, DrwMpAn.x, DrwMpAn.y, DrwMpAn.width, DrwMpAn.height, DrwMpAn.Scale, DrwMpAn.rotation);
		}
	}
	for (UINT i = 0; i < GVar_ClButton.MaxButton; i++) {
		if (GVar_ClButton.Button[i].ButtonType == Button_Dan) {

		}
		else if (GVar_ClButton.Button[i].ButtonType == Button_Duo) {
			ButtonImageInfo &pButon = GVar_ClButton.Button[i].MultiButton[GVar_ClButton.Button[i].SelectIndex];
			if (IsRectMouse(pButon.x, pButon.y, pButon.Width, pButon.Height, GVar_MousePoint)) {
				if (GVar_MousePoint.KeyState == WM_LBUTTONDOWN) {
					if (i < 5)GVar_玩家职业 = i;
					if (i < 9 && i > 5)GVar_玩家国家 = i - 6;
					switch (i)
					{
					//0-4职业按钮
					//5开始游戏按钮
					//6-8国家选择按钮
					//9-10性别选择按钮
					case 0:
						for (UINT i = 0; i < 5; i++) {
							GVar_ClButton.Button[i].SelectIndex = 0;
						}
						GVar_ClButton.Button[i].SelectIndex = 1;
						PlayerButton();
						break;
					case 1:
						for (UINT i = 0; i < 5; i++) {
							GVar_ClButton.Button[i].SelectIndex = 0;
						}
						GVar_ClButton.Button[i].SelectIndex = 1;
						PlayerButton();
						break;
					case 2:
						for (UINT i = 0; i < 5; i++) {
							GVar_ClButton.Button[i].SelectIndex = 0;
						}
						GVar_ClButton.Button[i].SelectIndex = 1;
						PlayerButton();
						break;
					case 3:
						for (UINT i = 0; i < 5; i++) {
							GVar_ClButton.Button[i].SelectIndex = 0;
						}
						GVar_ClButton.Button[i].SelectIndex = 1;
						PlayerButton();
						break;
					case 4:
						for (UINT i = 0; i < 5; i++) {
							GVar_ClButton.Button[i].SelectIndex = 0;
						}
						GVar_ClButton.Button[i].SelectIndex = 1;
						PlayerButton();
						break;
					case 5:
						GVar_ClButton.Button[i].SelectIndex = 2;
						if (ClickMouseDelay()){
							if (GVar_Ime.GetImeLen() < 1)SetTipDialogBoxText("   不允许使用空角色名!\r\n      请输入角色名!");
							else { 
								CopyMemory(GVar_玩家名字, GVar_Ime.GetImeString().c_str(), GVar_Ime.GetImeLen());
								GVar_DrawCreatPlyer = FALSE;
								ReleaseMapResource(GVar_MapImageInfo);
								ReleaseMapResource(GVar_PlayerImageInfo);
								ReleaseButtonResource(GVar_ClButton);
								ReleaseButtonResource(GVar_ClTipButton);
								GVar_DrawCityMappl = TRUE;
								return;
							}
						}
						break;
					case 6:
						for (UINT i = 6; i < 9; i++) {
							GVar_ClButton.Button[i].SelectIndex = 0;
						}
						GVar_ClButton.Button[i].SelectIndex = 1;
						break;
					case 7:
						for (UINT i = 6; i < 9; i++) {
							GVar_ClButton.Button[i].SelectIndex = 0;
						}
						GVar_ClButton.Button[i].SelectIndex = 1;
						break;
					case 8:
						for (UINT i = 6; i < 9; i++) {
							GVar_ClButton.Button[i].SelectIndex = 0;
						}
						GVar_ClButton.Button[i].SelectIndex = 1;
						break;
					case 9:
						for (UINT i = 9; i < 11; i++) {
							GVar_ClButton.Button[i].SelectIndex = 0;
						}
						GVar_ClButton.Button[i].SelectIndex = 1;
						GVar_玩家性别是否为男 = TRUE;
						PlayerButton();
						break;
					case 10:
						for (UINT i = 9; i < 11; i++) {
							GVar_ClButton.Button[i].SelectIndex = 0;
						}
						GVar_ClButton.Button[i].SelectIndex = 1;
						GVar_玩家性别是否为男 = FALSE;
						PlayerButton();
						break;
					default:
						break;
					}
					
				}
				else {
					if (i == 5)GVar_ClButton.Button[i].SelectIndex = 1;
				}
			}
			else {
				if (i == 5)GVar_ClButton.Button[i].SelectIndex = 0;
			}
			//D2Dx9.DrawRectagle(pButon.x, pButon.y, pButon.x + pButon.Width, pButon.y + pButon.Height, 1.f, D3DCOLOR_XRGB(255, 0, 0));
			GVar_D2Dx9.DrawTexture(pButon.Texture, pButon.x, pButon.y, pButon.Width, pButon.Height);
		}
	}
	//绘制输入框;
	GVar_Ime.GetIMEString(GVar_hWnd);
	string imestr = GVar_Ime.GetImeString();
	float ImeX = (CL_WinWidth / 2) - 60, ImeY = CL_WinHeight - 50;
	if (GetTickCount() - GVar_ImeTick > 1000) {
		if (GetTickCount() - GVar_ImeTick > 1800)GVar_ImeTick = GetTickCount();
	}
	else GVar_D2Dx9.DrawLineto(ImeX + (float)(imestr.length() * 8), ImeY - 15.f, ImeX + (imestr.length() * 8), ImeY, 1.f, D3DCOLOR_XRGB(255, 255, 255));
	RECT lFontRect = { (LONG)ImeX, (LONG)(ImeY - 18.f), CL_WinWidth,CL_WinHeight };
	GVar_D2Dx9.DrawTextFont(imestr.c_str(), 16, 1, FALSE, "隶书", &lFontRect);
	RECT lCareerRect = { CL_WinWidth - 180, 250, CL_WinWidth,CL_WinHeight };
	GVar_D2Dx9.DrawTextFont(GVar_szCareerTip, 12, 1, FALSE, "宋体", &lCareerRect, D3DCOLOR_XRGB(255, 255, 255));
}
void InitCityMapResource() {
	C_Module Mod;
	const char *szPath = Mod.GetCurrencyPathFileA("bajun.map");
	ResouceDataFile Resdatfile;
	ResouceDataFile::ResMapOInfo *MpImginfo = Resdatfile.GetMapImageInfo(szPath);
	if (!MpImginfo) MsgTipExit("程序加载资源出现未知错误!");
	if (GVar_MapImageInfo.MaxInt > 0)MsgTipExit("程序加载资源出现未知错误!");
	UINT MaxAllocLen = sizeof(DrawMapInfo)*MpImginfo->MaxCount;
	GVar_MapImageInfo.DrawMap = (PDrawMapInfo)LocalAlloc(LMEM_ZEROINIT, MaxAllocLen);
	GVar_MapImageInfo.MaxInt = MpImginfo->MaxCount;
	if (!GVar_MapImageInfo.DrawMap) MsgTipExit("申请内存失败,内存不足!");
	LoadMapResourceData(MpImginfo, GVar_MapImageInfo, MaxAllocLen, szPath);
	//加载人物动画;
	Resdatfile.Release();
	szPath = Mod.GetCurrencyPathFileA("ResMap.map");
	MpImginfo = Resdatfile.GetMapImageInfo(szPath);
	if (!MpImginfo) MsgTipExit("程序加载资源出现未知错误!");
	if (GVar_PlayerImageInfo.MaxInt > 0)MsgTipExit("程序加载资源出现未知错误!");
	MaxAllocLen = sizeof(DrawMapInfo)*MpImginfo->MaxCount;
	GVar_PlayerImageInfo.DrawMap = (PDrawMapInfo)LocalAlloc(LMEM_ZEROINIT, MaxAllocLen);
	GVar_PlayerImageInfo.MaxInt = MpImginfo->MaxCount;
	if (!GVar_PlayerImageInfo.DrawMap) MsgTipExit("申请内存失败,内存不足!");
	LoadMapResourceData(MpImginfo, GVar_PlayerImageInfo, MaxAllocLen, szPath);
}
//绘制城市地图;
void DrawCityMap() {
	if (!GVar_MapImageInfo.DrawMap)InitCityMapResource();
	//绘制显示UI;
	for (UINT i = 0; i < GVar_MapImageInfo.MaxInt; i++) {
		PDrawMapInfo DrwMp = &GVar_MapImageInfo.DrawMap[i];
		if (DrwMp->ImgLoadType == _Image) {
			GVar_D2Dx9.DrawTexture(DrwMp->Texture, DrwMp->x, DrwMp->y, DrwMp->width, DrwMp->height, DrwMp->Scale, DrwMp->rotation);
		}
		else if (DrwMp->ImgLoadType == Animate) {
			if (GetTickCount() - DrwMp->AnimateOldTick > DrwMp->AnimateDelay) {
				DrwMp->AnimateTickIndex++;
				DrwMp->AnimateOldTick = GetTickCount();
				if (DrwMp->AnimateTickIndex >= DrwMp->AnimateMaxCount)
					DrwMp->AnimateTickIndex = 0;
			}
			DrawMapInfo DrwMpAn = DrwMp->Animate[DrwMp->AnimateTickIndex];
			GVar_D2Dx9.DrawTexture(DrwMpAn.Texture, DrwMpAn.x, DrwMpAn.y, DrwMpAn.width, DrwMpAn.height, DrwMpAn.Scale, DrwMpAn.rotation);
		}
	}
	PDrawMapInfo DrwMp = &GVar_PlayerImageInfo.DrawMap[GVar_PlayerImageInfo.AnimateState];
	if (DrwMp->ImgLoadType == Animate) {
		if (GetTickCount() - DrwMp->AnimateOldTick > DrwMp->AnimateDelay) {
			DrwMp->AnimateTickIndex++;
			DrwMp->AnimateOldTick = GetTickCount();
			if (DrwMp->AnimateTickIndex >= DrwMp->AnimateMaxCount)
				DrwMp->AnimateTickIndex = 0;
		}
		DrawMapInfo DrwMpAn = DrwMp->Animate[DrwMp->AnimateTickIndex];
		GVar_D2Dx9.DrawTexture(DrwMpAn.Texture, DrwMpAn.x + GVar_CreenCentreX, DrwMpAn.y + GVar_CreenCentreY, DrwMpAn.width, DrwMpAn.height, DrwMpAn.Scale, DrwMpAn.rotation);
	}
}
