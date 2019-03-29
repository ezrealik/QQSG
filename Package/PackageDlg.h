
// PackageDlg.h: 头文件
//

#pragma once
//全局函数
void TimeEvent();
void RunClassFuntion(LPVOID This);
// CPackageDlg 对话框
class CPackageDlg : public CDialogEx
{
// 构造
public:
	CPackageDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PACKAGE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	int CPackageDlg::GetAllgpxFilepathFromfolder(WCHAR*  Path, HTREEITEM Parant);
	void CPackageDlg::BrowserSoftware();
	CEdit Edit_PathDirect;
	CTreeCtrl ListFile;
	//CImageList nImageList;
	afx_msg void OnClose();
	CButton Button_Browser;
	afx_msg void OnBnClickedButton2();
	CButton Button_Package;
};
