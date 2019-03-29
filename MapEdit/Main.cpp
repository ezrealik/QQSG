#include "Main.h"
#define ThreadMutexLockDeleay 3500
DirectX92D D2Dx9;
HWND G_hWnd;
BOOL DrawThread = TRUE;
WNDPROC OldDrawWndProc = nullptr, OldEditWndProc = nullptr;
MapImageInfo ImageInfo = { 0 };
MouseInfo MousePoint = { 0 };
HINSTANCE G_hInst;
HANDLE G_lpDrawThread = nullptr;
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	G_hInst = hInstance;
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)WndProcc, NULL);
	return 0;
}
RECT GlRect, ClRect, lRect;
BOOL CALLBACK WndProcc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_INITDIALOG: {
		UINT x = GetSystemMetrics(SM_CXSCREEN), y = GetSystemMetrics(SM_CYSCREEN);
		if (x < 1296 && y < 780) {
			if (MessageBox(hWnd, L"屏幕分辨率小于窗口分辨率,继续打开可能导致部分窗口无法显示!", NULL, MB_YESNO | MB_ICONERROR) == IDYES) {
				ExitProcess(NULL);
			}
		}
		HWND MapHwnd = GetDlgItem(hWnd, IDC_MapShow);
		MoveWindow(MapHwnd, 5, 3, 1024, 768, FALSE);
		G_hWnd = hWnd;
		GetClientRect(MapHwnd, &lRect);
		UINT Width = lRect.right - lRect.left;
		UINT Height = lRect.bottom - lRect.top;
		SendMessage(GetDlgItem(hWnd, IDC_CHECK_IsRectangle), BM_SETCHECK, BST_CHECKED, NULL);
		SendMessage(GetDlgItem(hWnd, IDC_CHECK_PlayAnimate), BM_SETCHECK, BST_CHECKED, NULL);
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_LoadStyle), CB_ADDSTRING, NULL, (LPARAM)L"图片模式");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_LoadStyle), CB_ADDSTRING, NULL, (LPARAM)L"动画模式");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_LoadStyle), CB_SETCURSEL, 0, NULL);
		if (!D2Dx9.InitD3D(MapHwnd, Width, Height, false)) { MessageBox(hWnd, L"初始化D3D失败!", NULL, MB_OK); ExitProcess(NULL); }
		OldDrawWndProc = (WNDPROC)SetWindowLong(MapHwnd, GWL_WNDPROC, (LONG)DrawWndProc);
		OldEditWndProc = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, IDC_EDIT_Scale), GWL_WNDPROC, (LONG)EditScaleWndProc);
		DrawThread = TRUE;
		//重置坐标原点;
		GetWindowRect(G_hWnd, &GlRect);
		GetWindowRect(GetDlgItem(G_hWnd, IDC_MapShow), &ClRect);
		RunDrawD3D();
		break;
	}
	case WM_CLOSE:
		DrawThread = FALSE;
		EndDialog(hWnd, NULL);
		ExitProcess(NULL);
		break;
	case WM_MOUSEMOVE:
		MousePoint.KeyCode = wParam;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONDOWN:
		MousePoint.KeyCode = wParam;
		MousePoint.KeyState = WM_LBUTTONDOWN;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_RBUTTONDOWN:
		MousePoint.KeyCode = wParam;
		MousePoint.KeyState = WM_RBUTTONDOWN;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONUP:
		MousePoint.KeyCode = wParam;
		MousePoint.KeyState = WM_LBUTTONUP;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_RBUTTONUP:
		MousePoint.KeyState = WM_RBUTTONUP;
		MousePoint.KeyCode = wParam;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		GetClientRect(GetDlgItem(G_hWnd, IDC_MapShow), &lRect);
		if (MousePoint.x > lRect.left && MousePoint.x < lRect.right &&
			MousePoint.y > lRect.top && MousePoint.y < lRect.bottom) {
			HMENU hMenu = LoadMenu(G_hInst, MAKEINTRESOURCE(IDR_MENU1));
			HMENU hSubMenu = GetSubMenu(hMenu, NULL);
			POINT Mouse;
			GetCursorPos(&Mouse);
			TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, Mouse.x, Mouse.y, NULL, hWnd, NULL);
			DestroyMenu(hMenu);
		}
		break;
	case WM_MBUTTONDOWN:
		MousePoint.KeyCode = wParam;
		MousePoint.KeyState = WM_MBUTTONDOWN;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_MBUTTONUP:
		MousePoint.KeyCode = wParam;
		MousePoint.KeyState = WM_MBUTTONUP;
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ImageFile:
			CallThreadFunction(ImportImage);
			break;
		case IDC_BUTTON_AnimateImage:
			CallThreadFunction(ImportAnimateImage);

			break;
		case ID_MENU_40001:
			CallThreadFunction(LayerUpImage);
			break;
		case ID_MENU_40002:
			CallThreadFunction(LayerNextImage);
			break;
		case ID_MENU_40003:
			CallThreadFunction(DeleteImageInfo);
			break;
		case ID_MENU_40004:
			CallThreadFunction(DeleteAllImage);
			break;
		case ID_MENU_40007:
			CallThreadFunction(LayerTopImage);
			break;
		case ID_MENU_40008:
			CallThreadFunction(LayerBottomImage);
			break;
		case ID_MENU_40009:
			CallThreadFunction(UpAnimateFrams);
			break;
		case ID_MENU_40010:
			CallThreadFunction(NextAnimateFrams);
			break;
		case ID_MENU_40011:
			CallThreadFunction(TopAnimateFrams);
			break;
		case ID_MENU_40012:
			CallThreadFunction(BottomAnimateFrams);
			break;
		default:
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
//绘制窗口处理函数;
HRESULT CALLBACK DrawWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_NCHITTEST:
		MousePoint.x = GET_X_LPARAM(lParam);
		MousePoint.y = GET_Y_LPARAM(lParam);
		break;
	case WM_DROPFILES: {
		HDROP hDropFile = (HDROP)wParam;
		UINT nFile = DragQueryFile(hDropFile, -1, NULL, 0);
		//通知绘制线程退出
		if (nFile > 0)CloseDrawD3D();
		else { DragFinish(hDropFile); break; }
		for (UINT i = 0; i < nFile; i++) {
			TCHAR szFileName[_MAX_PATH] = { 0 };
			if (DragQueryFile(hDropFile, i, szFileName, _MAX_PATH)) {
				if (ImageInfo.MaxImage > 0) {
					UINT MaxI = ImageInfo.MaxImage + 1;
					void *pAlloc = LocalAlloc(LMEM_ZEROINIT, sizeof(ImageTexturInfo)*MaxI);
					if (!pAlloc) { MessageBox(NULL, L"内存不足,申请内存失败!", NULL, NULL); ExitProcess(NULL); }
					ZeroMemory(pAlloc, sizeof(ImageTexturInfo)*MaxI);
					CopyMemory(pAlloc, ImageInfo.Image, sizeof(ImageTexturInfo)*ImageInfo.MaxImage);
					LocalFree(ImageInfo.Image);
					ImageInfo.Image = (PImageTexturInfo)pAlloc;
					ImageInfo.MaxImage = MaxI;
					ImageInfo.Image[ImageInfo.MaxImage - 1].Texture = D2Dx9.LoadTexture(szFileName);
					CopyMemory(ImageInfo.Image[ImageInfo.MaxImage - 1].ImageFile, szFileName, _MAX_PATH);
					DirectX92D::ImageInfo ImgInfo = D2Dx9.GetImageInfo(szFileName);
					ImageInfo.Image[ImageInfo.MaxImage - 1].Width = ImgInfo.Width;
					ImageInfo.Image[ImageInfo.MaxImage - 1].Height = ImgInfo.Height;
					ImageInfo.Image[ImageInfo.MaxImage - 1].Scale = 1.0f;
					ImageInfo.Image[ImageInfo.MaxImage - 1].RectAngleColor = D3DCOLOR_XRGB(255, 255, 255);
					ImageInfo.Image[ImageInfo.MaxImage - 1].ImgLoadType = Image;
				}
				else {
					void *pAlloc = LocalAlloc(LMEM_ZEROINIT, sizeof(ImageTexturInfo));
					if (!pAlloc) { MessageBox(NULL, L"内存不足,申请内存失败!", NULL, NULL); ExitProcess(NULL); }
					ZeroMemory(pAlloc, sizeof(ImageTexturInfo));
					ImageInfo.Image = (PImageTexturInfo)pAlloc;
					ImageInfo.MaxImage = 1;
					ImageInfo.Image[ImageInfo.MaxImage - 1].Texture = D2Dx9.LoadTexture(szFileName);
					CopyMemory(ImageInfo.Image[ImageInfo.MaxImage - 1].ImageFile, szFileName, _MAX_PATH);
					DirectX92D::ImageInfo ImgInfo = D2Dx9.GetImageInfo(szFileName);
					ImageInfo.Image[ImageInfo.MaxImage - 1].Width = ImgInfo.Width;
					ImageInfo.Image[ImageInfo.MaxImage - 1].Height = ImgInfo.Height;
					ImageInfo.Image[ImageInfo.MaxImage - 1].Scale = 1.0f;
					ImageInfo.Image[ImageInfo.MaxImage - 1].RectAngleColor = D3DCOLOR_XRGB(255, 255, 255);
					ImageInfo.Image[ImageInfo.MaxImage - 1].ImgLoadType = Image;
				}
				OutputDebugString(szFileName);
				OutputDebugString(L"\n");
			}
		}
		DragFinish(hDropFile);
		RunDrawD3D();
		break;
	}
	default:
		break;
	}
	return CallWindowProc(OldDrawWndProc, hWnd, uMsg, wParam, lParam);
}
//D3D绘制线程;
UINT OldFpsTick = 0, FPSCount = 0, NewFPSCount = 0, SelectIndex = 0x8FFFFFF, SelectAnimateIndex = 0x8FFFFFF, CurrencyIndex = 0, UpIndex = 0x8FFFFFF;
MouseInfo BOMouse;
BOOL ISMove, IsTip;
void WINAPI DrawD3D() {
	while (DrawThread) {
		LPDIRECT3DDEVICE9 D3DDevice9 = D2Dx9.GetD3Devicex9();
		if (!D3DDevice9)continue;
		D3DDevice9->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		if (SUCCEEDED(D3DDevice9->BeginScene())) {
			UINT X = (ClRect.left - GlRect.left) - 10, Y = (ClRect.top - GlRect.top) - 32;
			for (UINT i = 0; i < ImageInfo.MaxImage; i++) {
				if(!ImageInfo.Image)break;
				//判断是动画帧还是背景图片;
				if (ImageInfo.Image[i].ImgLoadType == Image) {
					if (MousePoint.x - X > ImageInfo.Image[i].x && MousePoint.x + X < ImageInfo.Image[i].x + ImageInfo.Image[i].Width
						&& MousePoint.y - Y > ImageInfo.Image[i].y && MousePoint.y + Y < ImageInfo.Image[i].y + ImageInfo.Image[i].Height) {
						if (!ISMove)CurrencyIndex = i;
					}
				}
				//如果是动画帧
				else if (ImageInfo.Image[i].ImgLoadType == Animate) {
					PAnimateImage pAnimat = &ImageInfo.Image[i].Animate[ImageInfo.Image[i].AnimateTickIndex];
					if (MousePoint.x - X > pAnimat->x && MousePoint.x - X < pAnimat->x + pAnimat->Width
						&& MousePoint.y - Y > pAnimat->y && MousePoint.y - Y < pAnimat->y + pAnimat->Height) {
						if (!ISMove)CurrencyIndex = i;
					}
				}
			}
			//判断是否在图片选区中
			if (ImageInfo.MaxImage > CurrencyIndex) {
				if (!ImageInfo.Image)continue;
				if (ImageInfo.Image[CurrencyIndex].ImgLoadType == Image) {
					if (MousePoint.x - X > ImageInfo.Image[CurrencyIndex].x && MousePoint.x - X < ImageInfo.Image[CurrencyIndex].x + ImageInfo.Image[CurrencyIndex].Width
						&& MousePoint.y - Y > ImageInfo.Image[CurrencyIndex].y && MousePoint.y - Y < ImageInfo.Image[CurrencyIndex].y + ImageInfo.Image[CurrencyIndex].Height) {
						if (MousePoint.KeyCode == MK_LBUTTON && MousePoint.KeyState == WM_LBUTTONDOWN) {
							if (SelectIndex != CurrencyIndex) {
								//是否高亮选中图片;
								if (SelectIndex < ImageInfo.MaxImage) {
									ImageInfo.Image[SelectIndex].RectAngleColor = D3DCOLOR_XRGB(255, 255, 255);
									ImageInfo.Image[SelectIndex].HeightLight = FALSE;
								}
								ImageInfo.Image[CurrencyIndex].HeightLight = TRUE;
								ImageInfo.Image[CurrencyIndex].RectAngleColor = D3DCOLOR_XRGB(255, 0, 0);
								SelectIndex = CurrencyIndex;
								IsTip = TRUE;//是否提示一次当前选中图片信息;
							}
							ISMove = TRUE;//是否选中一个图片
							//记录鼠标选中是的坐标点
							if (BOMouse.OldTick == 0xF841F || BOMouse.OldTick == 0) {
								BOMouse.x = MousePoint.x - (int)ImageInfo.Image[CurrencyIndex].x;
								BOMouse.y = MousePoint.y - (int)ImageInfo.Image[CurrencyIndex].y;
								BOMouse.OldTick = 0x6F414;
							}
							//根据鼠标移动移动选中图片坐标x;
							ImageInfo.Image[CurrencyIndex].x = (float)MousePoint.x - BOMouse.x;
							//限制图片不超出绘制区
							if (ImageInfo.Image[CurrencyIndex].x < 0)ImageInfo.Image[CurrencyIndex].x = 0;
							else if (ImageInfo.Image[CurrencyIndex].x > (lRect.right - lRect.left) - ImageInfo.Image[CurrencyIndex].Width)
								ImageInfo.Image[CurrencyIndex].x = (float)(lRect.right - lRect.left) - ImageInfo.Image[CurrencyIndex].Width;
							//根据鼠标移动移动选中图片坐标y;
							ImageInfo.Image[CurrencyIndex].y = (float)MousePoint.y - BOMouse.y;
							//限制图片不超出绘制区
							if (ImageInfo.Image[CurrencyIndex].y < 0)ImageInfo.Image[CurrencyIndex].y = 0;
							else if (ImageInfo.Image[CurrencyIndex].y > (lRect.bottom - lRect.top) - ImageInfo.Image[CurrencyIndex].Height)
								ImageInfo.Image[CurrencyIndex].y = (float)(lRect.bottom - lRect.top) - ImageInfo.Image[CurrencyIndex].Height;
						}
						if (MousePoint.KeyCode == MK_RBUTTON && MousePoint.KeyState == WM_RBUTTONDOWN) {
							if (SelectIndex != CurrencyIndex) {
								//是否高亮选中图片;
								if (SelectIndex < ImageInfo.MaxImage) {
									ImageInfo.Image[SelectIndex].RectAngleColor = D3DCOLOR_XRGB(255, 255, 255);
									ImageInfo.Image[SelectIndex].HeightLight = FALSE;
								}
								ImageInfo.Image[CurrencyIndex].HeightLight = TRUE;
								ImageInfo.Image[CurrencyIndex].RectAngleColor = D3DCOLOR_XRGB(255, 0, 0);
								SelectIndex = CurrencyIndex;
								IsTip = TRUE;//是否提示一次当前选中图片信息;
							}
						}
					}
				}
				else if (ImageInfo.Image[CurrencyIndex].ImgLoadType == Animate) {
					AnimateImage pAnimat = ImageInfo.Image[CurrencyIndex].Animate[ImageInfo.Image[CurrencyIndex].AnimateTickIndex];
					if (MousePoint.x - X > pAnimat.x && MousePoint.x - X < pAnimat.x + pAnimat.Width
						&& MousePoint.y - Y > pAnimat.y && MousePoint.y - Y < pAnimat.y + pAnimat.Height) {
						if (MousePoint.KeyCode == MK_LBUTTON && MousePoint.KeyState == WM_LBUTTONDOWN) {
							if (SelectIndex != CurrencyIndex|| SelectAnimateIndex != ImageInfo.Image[CurrencyIndex].AnimateTickIndex) {
								//是否高亮选中图片;
								if (SelectIndex < ImageInfo.MaxImage) {
									ImageInfo.Image[SelectIndex].RectAngleColor = D3DCOLOR_XRGB(255, 255, 255);
									ImageInfo.Image[SelectIndex].HeightLight = FALSE;
								}
								ImageInfo.Image[CurrencyIndex].HeightLight = TRUE;
								ImageInfo.Image[CurrencyIndex].RectAngleColor = D3DCOLOR_XRGB(255, 0, 0);
								SelectIndex = CurrencyIndex;
								SelectAnimateIndex = ImageInfo.Image[CurrencyIndex].AnimateTickIndex;
								IsTip = TRUE;//是否提示一次当前选中图片信息;
							}
							ISMove = TRUE;//是否选中一个图片
							//记录鼠标选中是的坐标点
							if (BOMouse.OldTick == 0xF841F || BOMouse.OldTick == 0) {
								BOMouse.x = MousePoint.x - (int)pAnimat.x;
								BOMouse.y = MousePoint.y - (int)pAnimat.y;
								BOMouse.OldTick = 0x6F414;
							}
							//根据鼠标移动移动选中图片坐标x;
							PAnimateImage pAnimat2 = &ImageInfo.Image[CurrencyIndex].Animate[ImageInfo.Image[CurrencyIndex].AnimateTickIndex];
							pAnimat2->x = (float)MousePoint.x - BOMouse.x;
							//限制图片不超出绘制区
							if (pAnimat2->x < 0) { 
								pAnimat2->x = 0; 
							}
							else if (pAnimat2->x > (lRect.right - lRect.left) - pAnimat2->Width)
								pAnimat2->x = (float)(lRect.right - lRect.left) - pAnimat2->Width;
							//根据鼠标移动移动选中图片坐标y;
							pAnimat2->y = (float)MousePoint.y - BOMouse.y;
							//限制图片不超出绘制区
							if (pAnimat2->y < 0)pAnimat2->y = 0;
							else if (pAnimat2->y > (lRect.bottom - lRect.top) - pAnimat2->Height)
								pAnimat2->y = (float)(lRect.bottom - lRect.top) - pAnimat2->Height;
						}
						if (MousePoint.KeyCode == MK_RBUTTON && MousePoint.KeyState == WM_RBUTTONDOWN) {
							if (SelectIndex != CurrencyIndex) {
								//是否高亮选中图片;
								if (SelectIndex < ImageInfo.MaxImage) {
									ImageInfo.Image[SelectIndex].RectAngleColor = D3DCOLOR_XRGB(255, 255, 255);
									ImageInfo.Image[SelectIndex].HeightLight = FALSE;
								}
								ImageInfo.Image[CurrencyIndex].HeightLight = TRUE;
								ImageInfo.Image[CurrencyIndex].RectAngleColor = D3DCOLOR_XRGB(255, 0, 0);
								SelectIndex = CurrencyIndex;
								IsTip = TRUE;//是否提示一次当前选中图片信息;
							}
						}
						if (MousePoint.KeyCode == MK_MBUTTON && MousePoint.KeyState == WM_MBUTTONDOWN) {
							//记录鼠标选中是的坐标点
							if (BOMouse.OldTick == 0xF841F || BOMouse.OldTick == 0) {
								BOMouse.x = MousePoint.x - (int)pAnimat.x;
								BOMouse.y = MousePoint.y - (int)pAnimat.y;
								BOMouse.OldTick = 0x6F414;
							}
							for (UINT i = 0; i < ImageInfo.Image[CurrencyIndex].AnimateMaxCout; i++) {
								//根据鼠标移动移动选中图片坐标x;
								PAnimateImage pAnimat2 = &ImageInfo.Image[CurrencyIndex].Animate[i];
								pAnimat2->x = (float)MousePoint.x - BOMouse.x;
								//限制图片不超出绘制区
								if (pAnimat2->x < 0) {
									pAnimat2->x = 0;
								}
								else if (pAnimat2->x > (lRect.right - lRect.left) - pAnimat2->Width)
									pAnimat2->x = (float)(lRect.right - lRect.left) - pAnimat2->Width;
								//根据鼠标移动移动选中图片坐标y;
								pAnimat2->y = (float)MousePoint.y - BOMouse.y;
								//限制图片不超出绘制区
								if (pAnimat2->y < 0)pAnimat2->y = 0;
								else if (pAnimat2->y > (lRect.bottom - lRect.top) - pAnimat2->Height)
									pAnimat2->y = (float)(lRect.bottom - lRect.top) - pAnimat2->Height;
							}
						}
					}
				}
			}
			//判断是否释放选中图片
			if (MousePoint.KeyState == WM_LBUTTONUP || MousePoint.KeyState == WM_RBUTTONUP|| MousePoint.KeyState == WM_MBUTTONUP) {
				ISMove = FALSE;
				BOMouse.x = 0;
				BOMouse.y = 0;
				BOMouse.OldTick = 0xF841F;
			}
			//绘制所有图片
			for (UINT i = 0; i < ImageInfo.MaxImage; i++) {
				if (ImageInfo.Image != nullptr) {
					if (ImageInfo.Image[i].ImgLoadType == Image) {
						//选中图片是否高亮显示
						if (SendMessage(GetDlgItem(G_hWnd, IDC_CHECK_HeightLight), BM_GETCHECK, 0, 0) == BST_CHECKED) {
							if (ImageInfo.Image[i].HeightLight)
								D2Dx9.DrawTexture(ImageInfo.Image[i].Texture, ImageInfo.Image[i].x, ImageInfo.Image[i].y, ImageInfo.Image[i].Width, ImageInfo.Image[i].Height, ImageInfo.Image[i].Scale, 0.0f, D3DCOLOR_XRGB(255, 0, 0));
							else D2Dx9.DrawTexture(ImageInfo.Image[i].Texture, ImageInfo.Image[i].x, ImageInfo.Image[i].y, ImageInfo.Image[i].Width, ImageInfo.Image[i].Height, ImageInfo.Image[i].Scale);
						}
						else D2Dx9.DrawTexture(ImageInfo.Image[i].Texture, ImageInfo.Image[i].x, ImageInfo.Image[i].y, ImageInfo.Image[i].Width, ImageInfo.Image[i].Height, ImageInfo.Image[i].Scale);
						//选中图片边框是否显示
						if (SendMessage(GetDlgItem(G_hWnd, IDC_CHECK_IsRectangle), BM_GETCHECK, 0, 0) == BST_CHECKED)
							D2Dx9.DrawRectagle(ImageInfo.Image[i].x, ImageInfo.Image[i].y, ImageInfo.Image[i].x + ImageInfo.Image[i].Width, ImageInfo.Image[i].y + ImageInfo.Image[i].Height,
								1.0f, ImageInfo.Image[i].RectAngleColor);
					}
					else if (ImageInfo.Image[i].ImgLoadType == Animate) {
						if (SendMessage(GetDlgItem(G_hWnd, IDC_CHECK_PlayAnimate), BM_GETCHECK, 0, 0) == BST_CHECKED) {
							if (GetTickCount() - ImageInfo.Image[i].OldAnimateTick > ImageInfo.Image[i].AnimateDelay) {
								ImageInfo.Image[i].OldAnimateTick = GetTickCount();
								ImageInfo.Image[i].AnimateTickIndex++;
							}
						}
						if (ImageInfo.Image[i].AnimateMaxCout <= ImageInfo.Image[i].AnimateTickIndex) {
							ImageInfo.Image[i].AnimateTickIndex = 0;
						}
						AnimateImage pAnimat = ImageInfo.Image[i].Animate[ImageInfo.Image[i].AnimateTickIndex];
						//选中图片是否高亮显示
						if (SendMessage(GetDlgItem(G_hWnd, IDC_CHECK_HeightLight), BM_GETCHECK, 0, 0) == BST_CHECKED) {
							if (ImageInfo.Image[i].HeightLight) {
								D2Dx9.DrawTexture(pAnimat.Texture, pAnimat.x, pAnimat.y, pAnimat.Width, pAnimat.Height, pAnimat.Scale, 0.0f, D3DCOLOR_XRGB(255, 0, 0));
							}
							else D2Dx9.DrawTexture(pAnimat.Texture, pAnimat.x, pAnimat.y, pAnimat.Width, pAnimat.Height, pAnimat.Scale);
						}
						else D2Dx9.DrawTexture(pAnimat.Texture, pAnimat.x, pAnimat.y, pAnimat.Width, pAnimat.Height, pAnimat.Scale);
						//选中图片边框是否显示
						if (SendMessage(GetDlgItem(G_hWnd, IDC_CHECK_IsRectangle), BM_GETCHECK, 0, 0) == BST_CHECKED)
							D2Dx9.DrawRectagle(pAnimat.x, pAnimat.y, pAnimat.x + pAnimat.Width, pAnimat.y + pAnimat.Height, 1.0f, ImageInfo.Image[i].RectAngleColor);
					}
				}
			}
			//显示绘制帧数;
			if (OldFpsTick == 0 || GetTickCount() - OldFpsTick > 1100) {
				OldFpsTick = GetTickCount();
				NewFPSCount = FPSCount;
				FPSCount = 0;
			}
			else FPSCount++;
			char FPSOut[50];
			sprintf(FPSOut, "FPS:%d,X:%dY:%d", NewFPSCount, MousePoint.x, MousePoint.y);
			D2Dx9.DrawFont(FPSOut, 12, NULL, FALSE, "隶书", &lRect, D3DCOLOR_XRGB(255, 0, 0));
			D3DDevice9->EndScene();
			D3DDevice9->Present(NULL, NULL, NULL, NULL);
			if (IsTip) {
				char ImgInfo[256] = { 0 };
				if (ImageInfo.Image[SelectIndex].ImgLoadType == Image) {
					sprintf(ImgInfo, "图片X:%.2f\t\t图片Y:%.2f\r\n\r\n图片高:%d\t\t图片宽:%d\r\n\r\n图片索引:%d\t\t图片总数:%d\r\n", ImageInfo.Image[SelectIndex].x, ImageInfo.Image[SelectIndex].y,
						ImageInfo.Image[SelectIndex].Width, ImageInfo.Image[SelectIndex].Height, CurrencyIndex, ImageInfo.MaxImage);
					SetWindowTextA(GetDlgItem(G_hWnd, IDC_Label_Tip), ImgInfo);
					HWND EditHwnd = GetDlgItem(G_hWnd, IDC_EDIT_Path);
					SetWindowText(EditHwnd, ImageInfo.Image[SelectIndex].ImageFile);
					UINT Strlen = wcslen(ImageInfo.Image[SelectIndex].ImageFile);
					SendMessage(EditHwnd, EM_SETSEL, Strlen, Strlen);
					sprintf(ImgInfo, "%.2f", ImageInfo.Image[SelectIndex].Scale);
					SetWindowTextA(GetDlgItem(G_hWnd, IDC_EDIT_Scale), ImgInfo);
					SendMessage(GetDlgItem(G_hWnd, IDC_COMBO_LoadStyle), CB_SETCURSEL, 1, NULL);
				}
				else if (ImageInfo.Image[SelectIndex].ImgLoadType == Animate) {
					AnimateImage pAnimat = ImageInfo.Image[SelectIndex].Animate[ImageInfo.Image[SelectIndex].AnimateTickIndex];
					sprintf(ImgInfo, "图片X:%.2f\t\t图片Y:%.2f\r\n\r\n图片高:%d\t\t图片宽:%d\r\n\r\n动画索引:%d\t\t动画总帧:%d\r\n\r\n动画帧时:%dms/ts\t", pAnimat.x, pAnimat.y,
						pAnimat.Width, pAnimat.Height, ImageInfo.Image[SelectIndex].AnimateTickIndex, ImageInfo.Image[SelectIndex].AnimateMaxCout, ImageInfo.Image[SelectIndex].AnimateDelay);
					SetWindowTextA(GetDlgItem(G_hWnd, IDC_Label_Tip), ImgInfo);
					HWND EditHwnd = GetDlgItem(G_hWnd, IDC_EDIT_Path);
					SetWindowText(EditHwnd, pAnimat.ImageFile);
					UINT Strlen = wcslen(pAnimat.ImageFile);
					SendMessage(EditHwnd, EM_SETSEL, Strlen, Strlen);
					sprintf(ImgInfo, "%.2f", pAnimat.Scale);
					SetWindowTextA(GetDlgItem(G_hWnd, IDC_EDIT_Scale), ImgInfo);
					SendMessage(GetDlgItem(G_hWnd, IDC_COMBO_LoadStyle), CB_SETCURSEL, 0, NULL);
				}
				/*BYTE *Rgb = (BYTE*)&ImageInfo.Image[SelectIndex].RectAngleColor;
				sprintf(ImgInfo, "%d", Rgb[2]);
				SetWindowTextA(GetDlgItem(G_hWnd, IDC_EDIT_ColorR), ImgInfo);
				sprintf(ImgInfo, "%d", Rgb[1]);
				SetWindowTextA(GetDlgItem(G_hWnd, IDC_EDIT_ColorG), ImgInfo);
				sprintf(ImgInfo, "%d", Rgb[0]);
				SetWindowTextA(GetDlgItem(G_hWnd, IDC_EDIT_ColorB), ImgInfo);*/
				IsTip = FALSE;
			}
		}
		else D3DDevice9->EndScene();
		Sleep(5);
	}
}
//图片缩放比例窗口处理函数;
HRESULT CALLBACK EditScaleWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	char Buf[50] = { 0 };
	float fScale;
	switch (uMsg)
	{
	case WM_KEYUP:
		GetWindowTextA(hWnd, Buf, sizeof(Buf));
		fScale = (float)atof(Buf);
		if (fScale > 0.0f) {
			if (ImageInfo.Image[SelectIndex].ImgLoadType == Image) {
				ImageInfo.Image[SelectIndex].Scale = fScale;
			}
			else if (ImageInfo.Image[SelectIndex].ImgLoadType == Animate) {
				ImageInfo.Image[SelectIndex].Animate[ImageInfo.Image[SelectIndex].AnimateTickIndex].Scale = fScale;
			}
		}
		break;
	default:
		break;
	}
	return CallWindowProc(OldEditWndProc, hWnd, uMsg, wParam, lParam);
}
//删除当前选中图片;
void WINAPI DeleteImageInfo() {
	if (ImageInfo.MaxImage <= 0)return;
	CloseDrawD3D();
	if (ImageInfo.Image[SelectIndex].Texture)
		ImageInfo.Image[SelectIndex].Texture->Release();
	
	for (UINT i = SelectIndex; i < ImageInfo.MaxImage - 1; i++) {
		memcpy(&ImageInfo.Image[i], &ImageInfo.Image[i + 1], sizeof(ImageTexturInfo));
	}
	ImageInfo.MaxImage--;
	if (ImageInfo.MaxImage <= 0) {
		LocalFree(ImageInfo.Image);
		ImageInfo = { 0 };
	}
	RunDrawD3D();
}
//删除所有图片;
void WINAPI DeleteAllImage() {
	if (ImageInfo.MaxImage <= 0)return;
	CloseDrawD3D();
	for (UINT i = 0; i < ImageInfo.MaxImage; i++) {
		if (ImageInfo.Image[i].Texture)ImageInfo.Image[i].Texture->Release();
		ImageInfo.MaxImage--;
	}
	LocalFree(ImageInfo.Image);
	ImageInfo = { 0 };
	RunDrawD3D();
}
//将图片置上一层;
void WINAPI LayerUpImage() {
	if (ImageInfo.MaxImage < 2)return;
	if (SelectIndex > ImageInfo.MaxImage)return;
	CloseDrawD3D();
	ImageTexturInfo TmpImageInfo;
	CopyMemory(&TmpImageInfo, &ImageInfo.Image[SelectIndex], sizeof(ImageTexturInfo));
	CopyMemory(&ImageInfo.Image[SelectIndex], &ImageInfo.Image[SelectIndex + 1], sizeof(ImageTexturInfo));
	CopyMemory(&ImageInfo.Image[SelectIndex + 1], &TmpImageInfo, sizeof(ImageTexturInfo));
	RunDrawD3D();
}
//将图片置下一层;
void WINAPI LayerNextImage() {
	if (ImageInfo.MaxImage < 2)return;
	if (SelectIndex > ImageInfo.MaxImage)return;
	CloseDrawD3D();
	ImageTexturInfo TmpImageInfo;
	CopyMemory(&TmpImageInfo, &ImageInfo.Image[SelectIndex], sizeof(ImageTexturInfo));
	CopyMemory(&ImageInfo.Image[SelectIndex], &ImageInfo.Image[SelectIndex - 1], sizeof(ImageTexturInfo));
	CopyMemory(&ImageInfo.Image[SelectIndex - 1], &TmpImageInfo, sizeof(ImageTexturInfo));
	RunDrawD3D();
}
//将图片置顶层;
void WINAPI LayerTopImage() {
	if (ImageInfo.MaxImage < 2)return;
	if (SelectIndex > ImageInfo.MaxImage)return;
	CloseDrawD3D();
	ImageTexturInfo TmpImageInfo;
	CopyMemory(&TmpImageInfo, &ImageInfo.Image[SelectIndex], sizeof(ImageTexturInfo));
	CopyMemory(&ImageInfo.Image[SelectIndex], &ImageInfo.Image[ImageInfo.MaxImage - 1], sizeof(ImageTexturInfo));
	CopyMemory(&ImageInfo.Image[ImageInfo.MaxImage - 1], &TmpImageInfo, sizeof(ImageTexturInfo));
	RunDrawD3D();
}
//将图片置底层;
void WINAPI LayerBottomImage() {
	if (ImageInfo.MaxImage < 2)return;
	if (SelectIndex > ImageInfo.MaxImage)return;
	CloseDrawD3D();
	ImageTexturInfo TmpImageInfo;
	CopyMemory(&TmpImageInfo, &ImageInfo.Image[SelectIndex], sizeof(ImageTexturInfo));
	CopyMemory(&ImageInfo.Image[SelectIndex], &ImageInfo.Image[0], sizeof(ImageTexturInfo));
	CopyMemory(&ImageInfo.Image[0], &TmpImageInfo, sizeof(ImageTexturInfo));
	RunDrawD3D();
}
//启动绘制线程;
void WINAPI RunDrawD3D() {
	DrawThread = TRUE;
	CloseHandle(G_lpDrawThread);
	G_lpDrawThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DrawD3D, NULL, NULL, NULL);
	if (!G_lpDrawThread) { MessageBox(NULL, L"绘制线程启动失败!", NULL, MB_OK); ExitProcess(NULL); }
}
//关闭绘制线程;
void WINAPI CloseDrawD3D() {
	//通知绘制线程退出
	DrawThread = FALSE;
	//等待线程退出
	DWORD WaitObject = WaitForSingleObject(G_lpDrawThread, ThreadMutexLockDeleay);
	//如果等待线程超出时长,强制终止线程;
	if (WaitObject != WAIT_OBJECT_0) {
		DWORD ExitCode;
		GetExitCodeThread(G_lpDrawThread, &ExitCode);
		TerminateThread(G_lpDrawThread, ExitCode);
	}
}
//导入背景图片;
void WINAPI ImportImage() {
	OPENFILENAME open = { 0 };
	//设置最大选择文件缓冲区;
	TCHAR szOpenFileNames[80 * MAX_PATH] = { 0 };
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szFileName[MAX_PATH];
	TCHAR *p = nullptr;
	int nLen = 0;
	open.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	open.lStructSize = sizeof(open);
	open.lpstrFile = szOpenFileNames;
	open.nMaxFile = sizeof(szOpenFileNames);
	open.lpstrFile[0] = '\0';
	open.lpstrFilter = TEXT("位图文件(*.bmp | *.jpg | *.png)\0 * .jpg; *.bmp; *.png\0所有文件(*.*)\0 * .*\0");
	CloseDrawD3D();
	if (GetOpenFileName(&open)){
		//把第一个文件名前的复制到szPath,即:
		//如果只选了一个文件,就复制到最后一个'/'
		//如果选了多个文件,就复制到第一个NULL字符
		lstrcpyn(szPath, szOpenFileNames, open.nFileOffset);
		//当只选了一个文件时,下面这个NULL字符是必需的.
		//这里不区别对待选了一个和多个文件的情况
		nLen = lstrlen(szPath);
		//如果选了多个文件,则必须加上'//'
		if (szPath[nLen - 1] != '\\'){
			lstrcat(szPath, TEXT("\\"));
		}
		//把指针移到第一个文件
		p = szOpenFileNames + open.nFileOffset; 
		while (*p)
		{
			ZeroMemory(szFileName, sizeof(szFileName));
			wcscpy(szFileName, szPath);
			wcscat(szFileName, p);
			p += lstrlen(p) + 1;     //移至下一个文件
			OutputDebugString(szFileName);
			OutputDebugStringA("\n");
			if (ImageInfo.MaxImage > 0) {
				UINT MaxI = ImageInfo.MaxImage + 1;
				void *pAlloc = LocalAlloc(LMEM_ZEROINIT, sizeof(ImageTexturInfo)*MaxI);
				if (!pAlloc) { MessageBox(NULL, L"内存不足,申请内存失败!", NULL, NULL); ExitProcess(NULL); }
				ZeroMemory(pAlloc, sizeof(ImageTexturInfo)*MaxI);
				CopyMemory(pAlloc, ImageInfo.Image, sizeof(ImageTexturInfo)*ImageInfo.MaxImage);
				LocalFree(ImageInfo.Image);
				ImageInfo.Image = (PImageTexturInfo)pAlloc;
				ImageInfo.MaxImage = MaxI;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Texture = D2Dx9.LoadTexture(szFileName);
				CopyMemory(ImageInfo.Image[ImageInfo.MaxImage - 1].ImageFile, szFileName, _MAX_PATH);
				DirectX92D::ImageInfo ImgInfo = D2Dx9.GetImageInfo(szFileName);
				ImageInfo.Image[ImageInfo.MaxImage - 1].Width = ImgInfo.Width;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Height = ImgInfo.Height;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Scale = 1.0f;
				ImageInfo.Image[ImageInfo.MaxImage - 1].RectAngleColor = D3DCOLOR_XRGB(255, 255, 255);
				ImageInfo.Image[ImageInfo.MaxImage - 1].ImgLoadType = Image;
			}
			else {
				void *pAlloc = LocalAlloc(LMEM_ZEROINIT, sizeof(ImageTexturInfo));
				if (!pAlloc) { MessageBox(NULL, L"内存不足,申请内存失败!", NULL, NULL); ExitProcess(NULL); }
				ZeroMemory(pAlloc, sizeof(ImageTexturInfo));
				ImageInfo.Image = (PImageTexturInfo)pAlloc;
				ImageInfo.MaxImage = 1;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Texture = D2Dx9.LoadTexture(szFileName);
				CopyMemory(ImageInfo.Image[ImageInfo.MaxImage - 1].ImageFile, szFileName, _MAX_PATH);
				DirectX92D::ImageInfo ImgInfo = D2Dx9.GetImageInfo(szFileName);
				ImageInfo.Image[ImageInfo.MaxImage - 1].Width = ImgInfo.Width;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Height = ImgInfo.Height;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Scale = 1.0f;
				ImageInfo.Image[ImageInfo.MaxImage - 1].RectAngleColor = D3DCOLOR_XRGB(255, 255, 255);
				ImageInfo.Image[ImageInfo.MaxImage - 1].ImgLoadType = Image;
			}
		}
	}
	RunDrawD3D();
}
//导入动画图片;
void WINAPI ImportAnimateImage() {
	OPENFILENAME open = { 0 };
	//设置最大选择文件缓冲区;
	TCHAR szOpenFileNames[80 * MAX_PATH] = { 0 };
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szFileName[MAX_PATH];
	TCHAR *p = nullptr;
	int nLen = 0;
	open.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	open.lStructSize = sizeof(open);
	open.lpstrFile = szOpenFileNames;
	open.nMaxFile = sizeof(szOpenFileNames);
	open.lpstrFile[0] = '\0';
	open.lpstrFilter = TEXT("位图文件(*.bmp | *.jpg | *.png)\0 * .jpg; *.bmp; *.png\0所有文件(*.*)\0 * .*\0");
	CloseDrawD3D();
	if (GetOpenFileName(&open)) {
		//把第一个文件名前的复制到szPath,即:
		//如果只选了一个文件,就复制到最后一个'/'
		//如果选了多个文件,就复制到第一个NULL字符
		lstrcpyn(szPath, szOpenFileNames, open.nFileOffset);
		//当只选了一个文件时,下面这个NULL字符是必需的.
		//这里不区别对待选了一个和多个文件的情况
		nLen = lstrlen(szPath);
		//如果选了多个文件,则必须加上'//'
		if (szPath[nLen - 1] != '\\') {
			lstrcat(szPath, TEXT("\\"));
		}
		//把指针移到第一个文件
		p = szOpenFileNames + open.nFileOffset;
		if (ImageInfo.MaxImage > 0) {
			UINT MaxI = ImageInfo.MaxImage + 1;
			void *pAlloc = LocalAlloc(LMEM_ZEROINIT, sizeof(ImageTexturInfo)*MaxI);
			if (!pAlloc) { MessageBox(NULL, L"内存不足,申请内存失败!", NULL, NULL); ExitProcess(NULL); }
			ZeroMemory(pAlloc, sizeof(ImageTexturInfo)*MaxI);
			CopyMemory(pAlloc, ImageInfo.Image, sizeof(ImageTexturInfo)*ImageInfo.MaxImage);
			LocalFree(ImageInfo.Image);
			ImageInfo.Image = (PImageTexturInfo)pAlloc;
			ImageInfo.MaxImage = MaxI;
			ImageInfo.Image[ImageInfo.MaxImage - 1].RectAngleColor = D3DCOLOR_XRGB(255, 255, 255);
			ImageInfo.Image[ImageInfo.MaxImage - 1].ImgLoadType = Animate;
			ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateDelay = 200;
			ImageInfo.Image[ImageInfo.MaxImage - 1].OldAnimateTick = GetTickCount();
		}
		else {
			void *pAlloc = LocalAlloc(LMEM_ZEROINIT, sizeof(ImageTexturInfo));
			if (!pAlloc) { MessageBox(NULL, L"内存不足,申请内存失败!", NULL, NULL); ExitProcess(NULL); }
			ZeroMemory(pAlloc, sizeof(ImageTexturInfo));
			ImageInfo.Image = (PImageTexturInfo)pAlloc;
			ImageInfo.MaxImage = 1;
			ImageInfo.Image[ImageInfo.MaxImage - 1].RectAngleColor = D3DCOLOR_XRGB(255, 255, 255);
			ImageInfo.Image[ImageInfo.MaxImage - 1].ImgLoadType = Animate;
			ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateDelay = 200;
			ImageInfo.Image[ImageInfo.MaxImage - 1].OldAnimateTick = GetTickCount();
		}
		while (*p)
		{
			ZeroMemory(szFileName, sizeof(szFileName));
			wcscpy(szFileName, szPath);
			wcscat(szFileName, p);
			p += lstrlen(p) + 1;     //移至下一个文件
			OutputDebugString(szFileName);
			OutputDebugStringA("\n");
			if (ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout > 0) {
				UINT MaxCount = ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout + 1;
				void *pAnimateAlloc = LocalAlloc(LMEM_ZEROINIT, sizeof(AnimateImage)*MaxCount);
				if (!pAnimateAlloc) { MessageBox(NULL, L"内存不足,申请内存失败!", NULL, NULL); ExitProcess(NULL); }
				CopyMemory(pAnimateAlloc, ImageInfo.Image[ImageInfo.MaxImage - 1].Animate, sizeof(AnimateImage)*ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout);
				LocalFree(ImageInfo.Image[ImageInfo.MaxImage - 1].Animate);
				ImageInfo.Image[ImageInfo.MaxImage - 1].Animate = (PAnimateImage)pAnimateAlloc;
				ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout = MaxCount;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Animate[ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout - 1].Texture = D2Dx9.LoadTexture(szFileName);
				wcscpy(ImageInfo.Image[ImageInfo.MaxImage - 1].Animate[ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout - 1].ImageFile, szFileName);
				DirectX92D::ImageInfo ImgInfo = D2Dx9.GetImageInfo(szFileName);
				ImageInfo.Image[ImageInfo.MaxImage - 1].Animate[ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout - 1].Width = ImgInfo.Width;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Animate[ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout - 1].Height = ImgInfo.Height;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Animate[ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout - 1].Scale = 1.0f;
			}
			else {
				void *pAnimateAlloc = LocalAlloc(LMEM_ZEROINIT, sizeof(AnimateImage));
				if (!pAnimateAlloc) { MessageBox(NULL, L"内存不足,申请内存失败!", NULL, NULL); ExitProcess(NULL); }
				ImageInfo.Image[ImageInfo.MaxImage - 1].Animate = (PAnimateImage)pAnimateAlloc;
				ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout++;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Animate[ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout - 1].Texture = D2Dx9.LoadTexture(szFileName);
				wcscpy(ImageInfo.Image[ImageInfo.MaxImage - 1].Animate[ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout - 1].ImageFile, szFileName);
				DirectX92D::ImageInfo ImgInfo = D2Dx9.GetImageInfo(szFileName);
				ImageInfo.Image[ImageInfo.MaxImage - 1].Animate[ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout - 1].Width = ImgInfo.Width;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Animate[ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout - 1].Height = ImgInfo.Height;
				ImageInfo.Image[ImageInfo.MaxImage - 1].Animate[ImageInfo.Image[ImageInfo.MaxImage - 1].AnimateMaxCout - 1].Scale = 1.0f;
			}
		}
	}
	RunDrawD3D();
	OutputDebugStringA("结束\n");
}
//上一帧动画;
void WINAPI UpAnimateFrams() {
	if (ImageInfo.MaxImage < 1)return;
	if (ImageInfo.Image[SelectIndex].ImgLoadType == Animate && ImageInfo.Image[SelectIndex].AnimateMaxCout > 1) {
		ImageInfo.Image[SelectIndex].AnimateTickIndex--;
		if (ImageInfo.Image[SelectIndex].AnimateTickIndex < 0) {
			ImageInfo.Image[SelectIndex].AnimateTickIndex = 0;
		}
	}
}
//下一帧动画;
void WINAPI NextAnimateFrams() {
	if (ImageInfo.MaxImage < 1)return;
	if (ImageInfo.Image[SelectIndex].ImgLoadType == Animate && ImageInfo.Image[SelectIndex].AnimateMaxCout > 1) {
		ImageInfo.Image[SelectIndex].AnimateTickIndex++;
		if (ImageInfo.Image[SelectIndex].AnimateTickIndex > ImageInfo.Image[SelectIndex].AnimateMaxCout) {
			ImageInfo.Image[SelectIndex].AnimateTickIndex = 0;
		}
	}
	return;
}
//置顶帧动画;
void WINAPI TopAnimateFrams(){
	if (ImageInfo.MaxImage < 1)return;
	ImageInfo.Image[SelectIndex].AnimateTickIndex = ImageInfo.Image[SelectIndex].AnimateMaxCout - 1;
	return ;
}
//置底帧动画;
void WINAPI BottomAnimateFrams(){
	if (ImageInfo.MaxImage < 1)return;
	ImageInfo.Image[SelectIndex].AnimateTickIndex = 0;
	return ;
}
//置上一帧动画;
void WINAPI SetUpAnimateFrams() {

}
//置下一帧动画;
void WINAPI SetNextAnimateFrams() {

}
//启动线程
void WINAPI CallThreadFunction(void *FuncAddress) {
	HANDLE lThreadHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)FuncAddress, NULL, NULL, NULL);
	CloseHandle(lThreadHandle);
}