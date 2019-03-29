
// PackageDlg.cpp: 实现文件
//
#include "stdafx.h"
#include "Package.h"
#include "PackageDlg.h"
#include "afxdialogex.h"
#include "../QQSG/ResourceFile.h"
#pragma warning(disable:4996)
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//全局变量
HWND Gl_Hwnd;
BOOL IsExit = FALSE;
// CPackageDlg 对话框

CPackageDlg::CPackageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PACKAGE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPackageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_Dir, Edit_PathDirect);
	DDX_Control(pDX, IDC_TREE1, ListFile);
	DDX_Control(pDX, IDC_BUTTON1, Button_Browser);
	DDX_Control(pDX, IDC_BUTTON2, Button_Package);
}

BEGIN_MESSAGE_MAP(CPackageDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CPackageDlg::OnBnClickedButton1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON2, &CPackageDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CPackageDlg 消息处理程序

BOOL CPackageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_SHOW);
	Gl_Hwnd = this->GetSafeHwnd();
	// TODO: 在此添加额外的初始化代码

	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPackageDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
		WCHAR bot[] = L"꧁";
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPackageDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
int CPackageDlg::GetAllgpxFilepathFromfolder(WCHAR*  Path, HTREEITEM Parant)
{
	WCHAR szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	wcscpy(szFind, Path);
	wcscat(szFind, L"\\*.*");
	HANDLE hFind = FindFirstFile(szFind, &FindFileData);
	HTREEITEM RootItem = nullptr;
	if (INVALID_HANDLE_VALUE == hFind)return -1;
	do
	{
		if (IsExit)break;
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(FindFileData.cFileName, L".") != 0 && wcscmp(FindFileData.cFileName, L"..") != 0)
			{
				//发现子目录，递归之
				WCHAR szFile[MAX_PATH] = { 0 };
				wcscpy(szFile, Path);
				wcscat(szFile, L"\\");
				wcscat(szFile, FindFileData.cFileName);
				//获取应用程序图标
				//SHFILEINFO shfile = { 0 };
				//SHGetFileInfo(szFile, NULL, &shfile, sizeof(shfile), SHGFI_ICON);
				//int nImgIndex = nImageList.Add(shfile.hIcon);
				//int nImgIndex = 0;
				RootItem = ListFile.InsertItem(FindFileData.cFileName, NULL, NULL, Parant);
				GetAllgpxFilepathFromfolder(szFile, RootItem);
			}
		}
		else
		{
			WCHAR szFile[MAX_PATH] = { 0 };
			wcscpy(szFile, Path);
			wcscat(szFile, L"\\");
			wcscat(szFile, FindFileData.cFileName);
			//获取应用程序图标. 会占用很大的资源因此不再使用!
			//SHFILEINFO shfile = { 0 };
			//SHGetFileInfo(szFile, NULL, &shfile, sizeof(shfile), SHGFI_ICON);
			//int nImgIndex = nImageList.Add(shfile.hIcon);
			//int nImgIndex = 0;
			ListFile.InsertItem(FindFileData.cFileName, NULL, NULL, Parant);
		}
	} while (FindNextFile(hFind, &FindFileData));
	FindClose(hFind);
	return 0;
}
void CPackageDlg::OnBnClickedButton1()
{
	//创建枚举文件夹线程;
	HANDLE lpThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)RunClassFuntion, this, NULL, NULL);
	CloseHandle(lpThread);
	Button_Browser.EnableWindow(FALSE);
}
void RunClassFuntion(LPVOID This) {
	CPackageDlg*ThisPackDlg = (CPackageDlg*)This;
	ThisPackDlg->BrowserSoftware();
}

void CPackageDlg::BrowserSoftware() {
	ListFile.DeleteAllItems();
	//浏览文件夹;
	TCHAR szBuffer[1024] = { 0 };
	BROWSEINFO bi = { 0 };
	bi.hwndOwner = NULL;
	bi.pszDisplayName = szBuffer;
	bi.lpszTitle = L"请选择游戏目录:";
	bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_EDITBOX;
	bi.lpfn = NULL;
	bi.lParam = 0;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (!idl) { Button_Browser.EnableWindow(TRUE); return; }
	SHGetPathFromIDList(idl, szBuffer);
	Edit_PathDirect.SetWindowTextW(szBuffer);
	int slen = Edit_PathDirect.GetWindowTextLengthW();
	Edit_PathDirect.SetSel(slen, slen, TRUE);
	//遍历文件夹文件;
	UINT OldTick;
	float Tick;
	//设置提示定时器;
	::SetTimer(Gl_Hwnd, 1, 200, (TIMERPROC)TimeEvent);
	OldTick = GetTickCount();
	TimeEvent();
	GetAllgpxFilepathFromfolder(szBuffer, nullptr);
	Button_Browser.EnableWindow(TRUE);
	//枚举结束关闭定时器;
	::KillTimer(Gl_Hwnd,1);
	//计算枚举用时时长;
	Tick = (float)GetTickCount() - OldTick;
	Tick /= 1000;
	swprintf(szBuffer, L"文件枚举成功,用时[ %.2f/S ]", Tick);
	SetWindowText(szBuffer);

}
WCHAR TipStr[100] = L"枚举资源文件中";
void TimeEvent() {
	//如果提示文本长度小于25,则添加[...]等待提示;
	if (wcslen(TipStr) <= 25) {
		wcscat(TipStr, L".");
	}
	else {
		//文本长度大于25,重置提示文本.
		wcscpy(TipStr, L"枚举资源文件中");
	}
	SetWindowText(Gl_Hwnd, TipStr);
}

void TimeEvent2() {
	//如果提示文本长度小于25,则添加[...]等待提示;
	if (wcslen(TipStr) <= 25) {
		wcscat(TipStr, L".");
	}
	else {
		//文本长度大于25,重置提示文本.
		wcscpy(TipStr, L"打包资源文件中");
	}
	SetWindowText(Gl_Hwnd, TipStr);
}
void CPackageDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//告诉线程主进程即将退出;
	IsExit = TRUE;
	CDialogEx::OnClose();
}
void RunResPackage(LPVOID This) {
	CEdit *CEditDir = (CEdit*)This;
	char Dirtt[MAX_PATH] = { 0 };
	TCHAR szBuffer[1024] = { 0 };
	GetWindowTextA(GetDlgItem(Gl_Hwnd, IDC_EDIT_Dir), Dirtt, MAX_PATH);
	UINT OldTick;
	float Tick;
	//设置提示定时器;
	::SetTimer(Gl_Hwnd, 2, 200, (TIMERPROC)TimeEvent2);
	OldTick = GetTickCount();
	TimeEvent();
	ResouceDataFile Resfil;
	Resfil.PackageResouce(Dirtt);
	EnableWindow(GetDlgItem(Gl_Hwnd, IDC_BUTTON2), TRUE);
	//枚举结束关闭定时器;
	::KillTimer(Gl_Hwnd, 2);
	//计算枚举用时时长;
	Tick = (float)GetTickCount() - OldTick;
	Tick /= 1000;
	swprintf(szBuffer, L"资源文件打包完成,用时[ %.2f/S ]", Tick);
	SetWindowText(Gl_Hwnd, szBuffer);
}
void CPackageDlg::OnBnClickedButton2()
{
	//创建资源文件打包线程;
	Button_Package.EnableWindow(FALSE);
	HANDLE lpThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)RunResPackage, this, NULL, NULL);
	CloseHandle(lpThread);
}
