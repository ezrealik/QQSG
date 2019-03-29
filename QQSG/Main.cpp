#include "Main.h"

const WCHAR ClassName[] = L"QQSGDiy";
WCHAR ExePath[256] = { 0 };
HWND G_hWnd;
BOOL DrawGame = TRUE, DrawTip = FALSE, DrawCreatPlyer = TRUE;
void *BGM_Login1 = nullptr;
HSTREAM StreamBGM1;
DirectX92D D2Dx9;
ImeInput Ime;
MouseInfo MousePoint = { 0 };
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
UINT Countf = 0;
LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	char path[256] = { 0 };
	switch (uMsg)
	{
	case WM_CREATE: {
		CoInitialize(nullptr);
		Ime.SetMaxLen(14);
		GetExePathW(ExePath, sizeof(ExePath));
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
UINT OldFpsTick = 0, FPSCount = 0, NewFPSCount = 0;
void D3DGameRun() {
	while (DrawGame)
	{
		LPDIRECT3DDEVICE9 D3DDevice9 = D2Dx9.GetD3Devicex9();
		if (!D3DDevice9)return;
		D2Dx9.Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		if (SUCCEEDED(D3DDevice9->BeginScene())) {
			//计算每秒FPS帧数;
			if (OldFpsTick == 0 || GetTickCount() - OldFpsTick > 1100) {
				OldFpsTick = GetTickCount();
				NewFPSCount = FPSCount;
				FPSCount = 0;
			}
			else FPSCount++;
			//绘制提示框;
			if (DrawTip) {
				DrawTipBox();
			}
			if (DrawCreatPlyer) {
				DrawCreatePlayer();
			}
			char FPSOut[50];
			sprintf(FPSOut, "FPS:%d", NewFPSCount);
			RECT lrect = { CL_WinWidth - 60,10,CL_WinWidth,50 };
			D2Dx9.DrawFont(FPSOut, 12, NULL, FALSE, "隶书", &lrect, D3DCOLOR_XRGB(255, 0, 0));
			D3DDevice9->EndScene();
			D3DDevice9->Present(NULL, NULL, NULL, NULL);
		}
		Sleep(1);
	}

}
//绘制提示框;
void DrawTipBox() {
	UINT TipWidth = 500, TipHeight = 600, TipTop = 30;
	LPDIRECT3DTEXTURE9 TipBox = D2Dx9.LoadRectTexture(L"UIRes\\登录UI\\登录角色\\提示框-1.bmp", TipWidth, TipHeight);
	WCHAR SubmitT[3][256] = { L"UIRes\\登录UI\\登录角色\\确定-1.bmp",L"UIRes\\登录UI\\登录角色\\确定-2.bmp",L"UIRes\\登录UI\\登录角色\\确定-3.bmp" };
	LPDIRECT3DTEXTURE9 Submit[3] = { 0 };
	for (int i = 0; i < 3; i++) {
		Submit[i] = D2Dx9.LoadTexture(SubmitT[i]);
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
//玩家职业变量;
NPCImageInfo PlayerCarrorCreate = { 0 };
//绘制创建玩家职业(阴阳士);
void DrawPlayerCarrorYY(PNPCImageInfo _PlayerInfo, int PlayDelay) {
	if (!_PlayerInfo->TextureInfo)return;
	//延迟播放帧判断
	int NewTickCount = GetTickCount();
	if ((NewTickCount - _PlayerInfo->OldTickCount) > PlayDelay) {
		_PlayerInfo->OldTickCount = NewTickCount;
		if (_PlayerInfo->TextureInfo[0].MaxImage >= _PlayerInfo->CurrencyImage) {
			_PlayerInfo->CurrencyImage++;
			if (_PlayerInfo->CurrencyImage > _PlayerInfo[0].TextureInfo->MaxImage) {
				_PlayerInfo->CurrencyImage = 0;
			}
		}
	}
	if (Countf > (UINT)_PlayerInfo->TextureInfo[0].MaxImage) {
		Countf = 0;
	}
	_PlayerInfo->CurrencyImage = Countf;
	char Bot[256];
	sprintf(Bot, "当前帧数:%d", Countf);
	RECT eFontRect = { 115, 152 ,CL_WinWidth,CL_WinHeight };
	D2Dx9.DrawTextFont(Bot, 11, 1, FALSE, "隶书", &eFontRect);
	//绘制当前帧图片;
	D2Dx9.DrawImageTexture(_PlayerInfo->TextureInfo[_PlayerInfo->CurrencyImage].ImageFile, _PlayerInfo->TextureInfo[_PlayerInfo->CurrencyImage].x,
		_PlayerInfo->TextureInfo[_PlayerInfo->CurrencyImage].y, _PlayerInfo->TextureInfo[_PlayerInfo->CurrencyImage].width, _PlayerInfo->TextureInfo[_PlayerInfo->CurrencyImage].height,
		_PlayerInfo->TextureInfo[_PlayerInfo->CurrencyImage].Scale, _PlayerInfo->TextureInfo[_PlayerInfo->CurrencyImage].rotation, D3DCOLOR_XRGB(155, 0, 0));
}
//绘制玩家角色创建;
UINT OldTickCount = 0, PlayerIndex = 0, Country = 0;
BOOL IsMan = FALSE;
void DrawCreatePlayer() {
	if (OldTickCount == 0)OldTickCount = GetTickCount();
#pragma region 图片资源加载
	LPDIRECT3DTEXTURE9 BgImage = D2Dx9.LoadTexture(L"UIRes\\地图\\背景\\游泳池-1.bmp");
	LPDIRECT3DTEXTURE9 SgInputBoxLeft = D2Dx9.LoadTexture(L"\\UIRes\\登录UI\\输入框\\输入框左.bmp");
	LPDIRECT3DTEXTURE9 SgInputBoxMid = D2Dx9.LoadRectTexture(L"\\UIRes\\登录UI\\输入框\\输入框中.bmp", 140, 50);
	LPDIRECT3DTEXTURE9 SgInputBoxRight = D2Dx9.LoadTexture(L"\\UIRes\\登录UI\\输入框\\输入框右.bmp");
	LPDIRECT3DTEXTURE9 CreatePlayer = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\创建角色-1.bmp");
	LPDIRECT3DTEXTURE9 State_Wu[2];
	State_Wu[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\吴-1.bmp");
	State_Wu[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\吴-2.bmp");
	LPDIRECT3DTEXTURE9 State_Shu[2];
	State_Shu[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\蜀-1.bmp");
	State_Shu[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\蜀-2.bmp");
	LPDIRECT3DTEXTURE9 State_Wei[2];
	State_Wei[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\魏-1.bmp");
	State_Wei[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\魏-2.bmp");
	LPDIRECT3DTEXTURE9 Player_Man[2];
	Player_Man[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\男-1.bmp");
	Player_Man[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\男-2.bmp");
	LPDIRECT3DTEXTURE9 Player_Woman[2];
	Player_Woman[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\女-1.bmp");
	Player_Woman[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\女-2.bmp");
	LPDIRECT3DTEXTURE9 NextItem = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\下一项-2.bmp");
	LPDIRECT3DTEXTURE9 UpItem = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\上一项-2.bmp");
	LPDIRECT3DTEXTURE9 CreateLogo = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\logo-1.bmp");
	LPDIRECT3DTEXTURE9 js[2];
	js[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\剑侍-1.bmp");
	js[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\剑侍-2.bmp");
	LPDIRECT3DTEXTURE9 hj[2];
	hj[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\豪杰-1.bmp");
	hj[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\豪杰-2.bmp");
	LPDIRECT3DTEXTURE9 yy[2];
	yy[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\阴阳士-1.bmp");
	yy[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\阴阳士-2.bmp");
	LPDIRECT3DTEXTURE9 xs[2];
	xs[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\仙术士-1.bmp");
	xs[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\仙术士-2.bmp");
	LPDIRECT3DTEXTURE9 yx[2];
	yx[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\游侠-1.bmp");
	yx[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\游侠-2.bmp");
	LPDIRECT3DTEXTURE9 StartGame[3]; 
	StartGame[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\开始游戏-1.bmp");
	StartGame[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\开始游戏-2.bmp");
	StartGame[2] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\开始游戏-3.bmp");
	LPDIRECT3DTEXTURE9 了解更多[3];
	了解更多[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\了解更多-1.bmp");
	了解更多[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\了解更多-2.bmp");
	了解更多[2] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\了解更多-3.bmp");
	LPDIRECT3DTEXTURE9 ExitGame[3];
	ExitGame[0] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\退出游戏-1.bmp");
	ExitGame[1] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\退出游戏-2.bmp");
	ExitGame[2] = D2Dx9.LoadTexture(L"UIRes\\登录UI\\创建角色\\退出游戏-3.bmp");
#pragma endregion
	//创建角色背景绘制;
	D2Dx9.DrawTexture(BgImage, 0, 0, CL_WinWidth, CL_WinHeight, 1.28f);
	D2Dx9.DrawTexture(CreateLogo, 5, 5, 100, 50);
	D2Dx9.DrawTexture(CreatePlayer, CL_WinWidth - 210, 30);
	void *CareerInfo = LocalAlloc(LMEM_ZEROINIT, 1024);
	if (!CareerInfo) { MessageBox(0, L"内存不足!", nullptr, MB_OK); return; }
#pragma region 国家选择按钮绘制;
	//判断是否在魏国选择选区内
	if (MousePoint.x > (int)CL_WinWidth - 189 && MousePoint.x < (int)CL_WinWidth - 152 && MousePoint.y >(int)55 && MousePoint.y < (int)92) {
		if (MousePoint.KeyCode == MK_LBUTTON)Country = 0;
	}
	if (Country == 0)D2Dx9.DrawTexture(State_Wei[1], CL_WinWidth - 180, 55);
	else D2Dx9.DrawTexture(State_Wei[0], CL_WinWidth - 180, 55);
	//判断是否在蜀国选择选区内
	if (MousePoint.x > (int)CL_WinWidth - 140 && MousePoint.x < (int)CL_WinWidth - 102 && MousePoint.y >(int)55 && MousePoint.y < (int)92) {
		if (MousePoint.KeyCode == MK_LBUTTON)Country = 1;
	}
	if (Country == 1)D2Dx9.DrawTexture(State_Shu[1], CL_WinWidth - 130, 55);
	else D2Dx9.DrawTexture(State_Shu[0], CL_WinWidth - 130, 55);
	//判断是否在吴国选择选区内
	if (MousePoint.x > (int)CL_WinWidth - 90 && MousePoint.x < (int)CL_WinWidth - 52 && MousePoint.y >(int)55 && MousePoint.y < (int)92) {
		if (MousePoint.KeyCode == MK_LBUTTON)Country = 2;
	}
	if (Country == 2)D2Dx9.DrawTexture(State_Wu[1], CL_WinWidth - 80, 55);
	else D2Dx9.DrawTexture(State_Wu[0], CL_WinWidth - 80, 55);
#pragma endregion
	char Bot[256];
	sprintf(Bot, "x:%d,y:%d,y:%d", MousePoint.x, MousePoint.y, CL_WinWidth / 2 - 102);
	RECT eFontRect = { 115, 52 ,CL_WinWidth,CL_WinHeight };
	D2Dx9.DrawTextFont(Bot, 11, 1, FALSE, "隶书", &eFontRect);
#pragma region 玩家性别选择按钮
	//判断是否在男玩家选择选区内
	if (MousePoint.x > (int)CL_WinWidth - 174 && MousePoint.x < (int)CL_WinWidth - 134 && MousePoint.y >(int)110 && MousePoint.y < (int)145) {
		if (MousePoint.KeyCode == MK_LBUTTON)IsMan = TRUE;
	}
	if (IsMan)D2Dx9.DrawTexture(Player_Man[1], CL_WinWidth - 170, 112);
	else D2Dx9.DrawTexture(Player_Man[0], CL_WinWidth - 170, 112);
	//判断是否在女玩家选择选区内
	if (MousePoint.x > (int)CL_WinWidth - 110 && MousePoint.x < (int)CL_WinWidth - 60 && MousePoint.y >(int)110 && MousePoint.y < (int)145) {
		if (MousePoint.KeyCode == MK_LBUTTON)IsMan = FALSE;
	}
	if (!IsMan)D2Dx9.DrawTexture(Player_Woman[1], CL_WinWidth - 100, 112);
	else D2Dx9.DrawTexture(Player_Woman[0], CL_WinWidth - 100, 112);
#pragma endregion
	D2Dx9.DrawTexture(UpItem, CL_WinWidth - 165, 190);
	D2Dx9.DrawTexture(NextItem, CL_WinWidth - 55, 190);
#pragma region 职业选择按钮绘制
	//判断是否在阴阳士选择选区内
	if (MousePoint.x > (int)CL_WinWidth / 2 - 178 && MousePoint.x < (int)CL_WinWidth / 2 - 128 && MousePoint.y >(int)CL_WinHeight - 178 && MousePoint.y < (int)CL_WinHeight - 128) {
		if (MousePoint.KeyCode == MK_LBUTTON)PlayerIndex = 0;
	}
	if (PlayerIndex == 0)D2Dx9.DrawTexture(yy[1], CL_WinWidth / 2 - 180, CL_WinHeight - 150);
	else D2Dx9.DrawTexture(yy[0], CL_WinWidth / 2 - 180, CL_WinHeight - 150);
	//判断是否在剑侍选择选区内
	if (MousePoint.x > (int)CL_WinWidth / 2 - 102 && MousePoint.x < (int)CL_WinWidth / 2 - 52 && MousePoint.y >(int)CL_WinHeight - 178 && MousePoint.y < (int)CL_WinHeight - 128) {
		if (MousePoint.KeyCode == MK_LBUTTON)PlayerIndex = 1;
	}
	if (PlayerIndex == 1)D2Dx9.DrawTexture(js[1], CL_WinWidth / 2 - 100, CL_WinHeight - 150);
	else D2Dx9.DrawTexture(js[0], CL_WinWidth / 2 - 100, CL_WinHeight - 150);
	//判断是否在豪杰选择选区内
	if (MousePoint.x > (int)CL_WinWidth / 2 - 25 && MousePoint.x < (int)CL_WinWidth / 2 + 25 && MousePoint.y >(int)CL_WinHeight - 178 && MousePoint.y < (int)CL_WinHeight - 128) {
		if (MousePoint.KeyCode == MK_LBUTTON)PlayerIndex = 2;
	}
	if (PlayerIndex == 2)D2Dx9.DrawTexture(hj[1], CL_WinWidth / 2 - 20, CL_WinHeight - 150);
	else D2Dx9.DrawTexture(hj[0], CL_WinWidth / 2 - 20, CL_WinHeight - 150);
	//判断是否在仙术士选择选区内
	if (MousePoint.x > (int)CL_WinWidth / 2 + 54 && MousePoint.x < (int)CL_WinWidth / 2 + 104 && MousePoint.y >(int)CL_WinHeight - 178 && MousePoint.y < (int)CL_WinHeight - 128) {
		if (MousePoint.KeyCode == MK_LBUTTON)PlayerIndex = 3;
	}
	if (PlayerIndex == 3)D2Dx9.DrawTexture(xs[1], CL_WinWidth / 2 + 60, CL_WinHeight - 150);
	else D2Dx9.DrawTexture(xs[0], CL_WinWidth / 2 + 60, CL_WinHeight - 150);
	//判断是否在游侠选择选区内
	if (MousePoint.x > (int)CL_WinWidth / 2 + 133 && MousePoint.x < (int)CL_WinWidth / 2 + 183 && MousePoint.y >(int)CL_WinHeight - 178 && MousePoint.y < (int)CL_WinHeight - 128) {
		if (MousePoint.KeyCode == MK_LBUTTON)PlayerIndex = 4;
	}
	if (PlayerIndex == 4)D2Dx9.DrawTexture(yx[1], CL_WinWidth / 2 + 140, CL_WinHeight - 150);
	else D2Dx9.DrawTexture(yx[0], CL_WinWidth / 2 + 140, CL_WinHeight - 150);
	//职业介绍;
	switch (PlayerIndex)
	{
	case 0: {
		strcpy((char*)CareerInfo, "      阴阳士\r\n\r\n远程丶法术高攻丶施法慢");
		PImageTexturInfo pAlloc = (PImageTexturInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(ImageTexturInfo) * 36);
		if (!pAlloc) { MessageBox(NULL, L"内存不足,分配内存失败!", NULL, MB_OK), ExitProcess(NULL); }
		PlayerCarrorCreate.TextureInfo = pAlloc;
#pragma region 初始化女角色阴阳士信息
		pAlloc[0].MaxImage = 23;
		pAlloc[0].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-7.bmp";
		pAlloc[0].rotation = 0.0f;
		pAlloc[0].Scale = 1.f;
		pAlloc[0].width = 100;
		pAlloc[0].height = 100;
		pAlloc[0].x = 200;
		pAlloc[0].y = 100;

		pAlloc[1].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-8.bmp";
		pAlloc[1].rotation = 0.0f;
		pAlloc[1].Scale = 1.f;
		pAlloc[1].width = 100;
		pAlloc[1].height = 100;
		pAlloc[1].x = 280;
		pAlloc[1].y = 100;

		pAlloc[2].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-9.bmp";
		pAlloc[2].rotation = 0.0f;
		pAlloc[2].Scale = 1.f;
		pAlloc[2].width = 100;
		pAlloc[2].height = 100;
		pAlloc[2].x = 360;
		pAlloc[2].y = 130;

		pAlloc[3].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-10.bmp";
		pAlloc[3].rotation = 0.0f;
		pAlloc[3].Scale = 1.f;
		pAlloc[3].width = 100;
		pAlloc[3].height = 100;
		pAlloc[3].x = 440;
		pAlloc[3].y = 130;

		pAlloc[4].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-11.bmp";
		pAlloc[4].rotation = 0.0f;
		pAlloc[4].Scale = 1.f;
		pAlloc[4].width = 100;
		pAlloc[4].height = 100;
		pAlloc[4].x = 520;
		pAlloc[4].y = 130;

		pAlloc[5].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-12.bmp";
		pAlloc[5].rotation = 0.0f;
		pAlloc[5].Scale = 1.f;
		pAlloc[5].width = 100;
		pAlloc[5].height = 100;
		pAlloc[5].x = 550;
		pAlloc[5].y = 130;

		pAlloc[6].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-13.bmp";
		pAlloc[6].rotation = 0.0f;
		pAlloc[6].Scale = 1.f;
		pAlloc[6].width = 100;
		pAlloc[6].height = 100;
		pAlloc[6].x = 520;
		pAlloc[6].y = 130;

		pAlloc[7].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-14.bmp";
		pAlloc[7].rotation = 0.0f;
		pAlloc[7].Scale = 1.f;
		pAlloc[7].width = 100;
		pAlloc[7].height = 100;
		pAlloc[7].x = 480;
		pAlloc[7].y = 130;

		pAlloc[8].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-15.bmp";
		pAlloc[8].rotation = 0.0f;
		pAlloc[8].Scale = 1.f;
		pAlloc[8].width = 100;
		pAlloc[8].height = 100;
		pAlloc[8].x = 460;
		pAlloc[8].y = 130;

		pAlloc[9].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-16.bmp";
		pAlloc[9].rotation = 0.0f;
		pAlloc[9].Scale = 1.f;
		pAlloc[9].width = 100;
		pAlloc[9].height = 100;
		pAlloc[9].x = 430;
		pAlloc[9].y = 130;

		pAlloc[10].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-17.bmp";
		pAlloc[10].rotation = 0.0f;
		pAlloc[10].Scale = 1.f;
		pAlloc[10].width = 100;
		pAlloc[10].height = 100;
		pAlloc[10].x = 400;
		pAlloc[10].y = 130;

		pAlloc[11].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-18.bmp";
		pAlloc[11].rotation = 0.0f;
		pAlloc[11].Scale = 1.f;
		pAlloc[11].width = 100;
		pAlloc[11].height = 100;
		pAlloc[11].x = 370;
		pAlloc[11].y = 130;

		pAlloc[12].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-19.bmp";
		pAlloc[12].rotation = 0.0f;
		pAlloc[12].Scale = 1.f;
		pAlloc[12].width = 100;
		pAlloc[12].height = 100;
		pAlloc[12].x = 340;
		pAlloc[12].y = 130;

		pAlloc[13].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-20.bmp";
		pAlloc[13].rotation = 0.0f;
		pAlloc[13].Scale = 1.f;
		pAlloc[13].width = 100;
		pAlloc[13].height = 100;
		pAlloc[13].x = 340;
		pAlloc[13].y = 130;

		pAlloc[14].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-21.bmp";
		pAlloc[14].rotation = 0.0f;
		pAlloc[14].Scale = 1.f;
		pAlloc[14].width = 100;
		pAlloc[14].height = 100;
		pAlloc[14].x = 340;
		pAlloc[14].y = 130;

		pAlloc[15].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-22.bmp";
		pAlloc[15].rotation = 0.0f;
		pAlloc[15].Scale = 1.f;
		pAlloc[15].width = 100;
		pAlloc[15].height = 100;
		pAlloc[15].x = 340;
		pAlloc[15].y = 130;

		pAlloc[16].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-23.bmp";
		pAlloc[16].rotation = 0.0f;
		pAlloc[16].Scale = 1.f;
		pAlloc[16].width = 100;
		pAlloc[16].height = 100;
		pAlloc[16].x = 340;
		pAlloc[16].y = 130;

		pAlloc[17].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-24.bmp";
		pAlloc[17].rotation = 0.0f;
		pAlloc[17].Scale = 1.f;
		pAlloc[17].width = 100;
		pAlloc[17].height = 100;
		pAlloc[17].x = 340;
		pAlloc[17].y = 130;

		pAlloc[18].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-25.bmp";
		pAlloc[18].rotation = 0.0f;
		pAlloc[18].Scale = 1.f;
		pAlloc[18].width = 100;
		pAlloc[18].height = 100;
		pAlloc[18].x = 340;
		pAlloc[18].y = 130;

		pAlloc[19].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-26.bmp";
		pAlloc[19].rotation = 0.0f;
		pAlloc[19].Scale = 1.f;
		pAlloc[19].width = 100;
		pAlloc[19].height = 100;
		pAlloc[19].x = 340;
		pAlloc[19].y = 130;

		pAlloc[20].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-27.bmp";
		pAlloc[20].rotation = 0.0f;
		pAlloc[20].Scale = 1.f;
		pAlloc[20].width = 100;
		pAlloc[20].height = 100;
		pAlloc[20].x = 340;
		pAlloc[20].y = 130;

		pAlloc[21].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-28.bmp";
		pAlloc[21].rotation = 0.0f;
		pAlloc[21].Scale = 1.f;
		pAlloc[21].width = 100;
		pAlloc[21].height = 100;
		pAlloc[21].x = 340;
		pAlloc[21].y = 130;

		pAlloc[22].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-29.bmp";
		pAlloc[22].rotation = 0.0f;
		pAlloc[22].Scale = 1.f;
		pAlloc[22].width = 100;
		pAlloc[22].height = 100;
		pAlloc[22].x = 410;
		pAlloc[22].y = 130;

		pAlloc[23].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-30.bmp";
		pAlloc[23].rotation = 0.0f;
		pAlloc[23].Scale = 1.f;
		pAlloc[23].width = 100;
		pAlloc[23].height = 100;
		pAlloc[23].x = 410;
		pAlloc[23].y = 170;

		pAlloc[24].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-31.bmp";
		pAlloc[24].rotation = 0.0f;
		pAlloc[24].Scale = 1.f;
		pAlloc[24].width = 100;
		pAlloc[24].height = 100;
		pAlloc[24].x = 410;
		pAlloc[24].y = 170;

		pAlloc[25].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-32.bmp";
		pAlloc[25].rotation = 0.0f;
		pAlloc[25].Scale = 1.f;
		pAlloc[25].width = 100;
		pAlloc[25].height = 100;
		pAlloc[25].x = 410;
		pAlloc[25].y = 170;

		pAlloc[26].ImageFile = L"UIRes\\登录UI\\创建角色\\1332-33.bmp";
		pAlloc[26].rotation = 0.0f;
		pAlloc[26].Scale = 1.f;
		pAlloc[26].width = 100;
		pAlloc[26].height = 100;
		pAlloc[26].x = 410;
		pAlloc[26].y = 170;
#pragma endregion
		DrawPlayerCarrorYY(&PlayerCarrorCreate, 180);
		LocalFree(pAlloc);
		break;
	}
	case 1:
		strcpy((char*)CareerInfo, "      剑侍\r\n\r\n近战丶平均丶攻速快");
		break;
	case 2:
		strcpy((char*)CareerInfo, "      豪杰\r\n\r\n近战丶高防丶血厚防高");
		break;
	case 3:
		strcpy((char*)CareerInfo, "      仙术士\r\n\r\n远程丶法伤治疗丶施法慢");
		break;
	case 4:
		strcpy((char*)CareerInfo, "      游侠\r\n\r\n远程丶高攻丶血薄防低");
		break;
	default:
		break;
	}
	RECT lrect = { CL_WinWidth - 160,280,CL_WinWidth,CL_WinHeight };
	D2Dx9.DrawTextFont((char*)CareerInfo, 11, 1, FALSE, "黑体", &lrect, D3DCOLOR_XRGB(255, 255, 255));
#pragma endregion
#pragma region 按钮绘制
	//判断是否在开始游戏选择选区内
	if (MousePoint.x > (int)CL_WinWidth / 2 + 61 && MousePoint.x < (int)CL_WinWidth / 2 + 173 && MousePoint.y >(int)CL_WinHeight - 95 && MousePoint.y < (int)CL_WinHeight - 68) {
		if (MousePoint.KeyCode == MK_LBUTTON){ D2Dx9.DrawTexture(StartGame[0], CL_WinWidth / 2 + 70, CL_WinHeight - 60); }
		else D2Dx9.DrawTexture(StartGame[2], CL_WinWidth / 2 + 70, CL_WinHeight - 60);
	}
	else D2Dx9.DrawTexture(StartGame[1], CL_WinWidth / 2 + 70, CL_WinHeight - 60);
	//判断是否在了解更多选择选区内
	if (MousePoint.x > (int)CL_WinWidth - 134 && MousePoint.x < (int)CL_WinWidth - 32 && MousePoint.y >(int)CL_WinHeight - 123 && MousePoint.y < (int)CL_WinHeight - 100) {
		if (MousePoint.KeyCode == MK_LBUTTON) { D2Dx9.DrawTexture(了解更多[0], CL_WinWidth - 120, CL_WinHeight - 90); }
		else D2Dx9.DrawTexture(了解更多[2], CL_WinWidth - 120, CL_WinHeight - 90);
	}
	else D2Dx9.DrawTexture(了解更多[1], CL_WinWidth - 120, CL_WinHeight - 90);
	//判断是否在退出游戏选择选区内
	if (MousePoint.x > (int)CL_WinWidth - 134 && MousePoint.x < (int)CL_WinWidth - 32 && MousePoint.y >(int)CL_WinHeight - 85 && MousePoint.y < (int)CL_WinHeight - 65) {
		if (MousePoint.KeyCode == MK_LBUTTON) { D2Dx9.DrawTexture(ExitGame[0], CL_WinWidth - 120, CL_WinHeight - 50); }
		else D2Dx9.DrawTexture(ExitGame[2], CL_WinWidth - 120, CL_WinHeight - 50);
	}
	else D2Dx9.DrawTexture(ExitGame[1], CL_WinWidth - 120, CL_WinHeight - 50);
#pragma endregion
#pragma region 角色名输入框绘制;
	D2Dx9.DrawTexture(SgInputBoxLeft, CL_WinWidth / 2 - 167, CL_WinHeight - 70, 50, 50, 1.3f);
	D2Dx9.DrawTexture(SgInputBoxMid, CL_WinWidth / 2 - 120, CL_WinHeight - 70);
	D2Dx9.DrawTexture(SgInputBoxRight, CL_WinWidth / 2 + 18, CL_WinHeight - 70, 50, 50, 1.3f);
	UINT NewTickCount = GetTickCount();
	if (NewTickCount - OldTickCount > 1000) {
		if (NewTickCount - OldTickCount > 1700) {
			OldTickCount = GetTickCount();
		}
	}
	else D2Dx9.DrawLineto((float)CL_WinWidth / 2 - 115 + (Ime.GetImeString().length() * 9), (float)CL_WinHeight - 52, (float)CL_WinWidth / 2 - 115+ (Ime.GetImeString().length() * 9), (float)CL_WinHeight - 36);
	RECT FontRect = { CL_WinWidth / 2 - 115, CL_WinHeight - 52 ,CL_WinWidth,CL_WinHeight };
	Ime.GetIMEString(G_hWnd);
	D2Dx9.DrawTextFont(Ime.GetImeString().c_str(), 18, 1, FALSE, "隶书", &FontRect, D3DCOLOR_XRGB(255, 155, 155));
#pragma endregion
#pragma region 加载的图片资源释放
	BgImage->Release();
	CreateLogo->Release();
	CreatePlayer->Release();
	State_Wei[0]->Release();
	State_Wei[1]->Release();
	State_Shu[0]->Release();
	State_Shu[1]->Release();
	State_Wu[0]->Release();
	State_Wu[1]->Release();
	Player_Man[0]->Release();
	Player_Man[1]->Release();
	Player_Woman[0]->Release();
	Player_Woman[1]->Release();
	UpItem->Release();
	NextItem->Release();
	yy[0]->Release();
	yy[1]->Release();
	js[0]->Release();
	js[1]->Release();
	hj[0]->Release();
	hj[1]->Release();
	xs[0]->Release();
	xs[1]->Release();
	yx[0]->Release();
	yx[1]->Release();
	SgInputBoxLeft->Release();
	SgInputBoxMid->Release();
	SgInputBoxRight->Release();
	StartGame[0]->Release();
	StartGame[1]->Release();
	StartGame[2]->Release();
	了解更多[0]->Release();
	了解更多[1]->Release();
	了解更多[2]->Release();
	ExitGame[0]->Release();
	ExitGame[1]->Release();
	ExitGame[2]->Release();
#pragma endregion
	LocalFree(CareerInfo);
}
