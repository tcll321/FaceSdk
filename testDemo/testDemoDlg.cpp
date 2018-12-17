
// testDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "testDemo.h"
#include "testDemoDlg.h"
#include "afxdialogex.h"
#include "FaceSdk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CtestDemoDlg 对话框



CtestDemoDlg::CtestDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtestDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SEX, m_edtSex);
	DDX_Control(pDX, IDC_EDIT_AGE, m_edtAge);
}

BEGIN_MESSAGE_MAP(CtestDemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CtestDemoDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_ACTION, &CtestDemoDlg::OnBnClickedBtnAction)
END_MESSAGE_MAP()


// CtestDemoDlg 消息处理程序

BOOL CtestDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CtestDemoDlg::OnPaint()
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
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CtestDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtestDemoDlg::OnCallbackInfo(int device, int sex, int age, int id, void* userParam)
{
	CtestDemoDlg* pdlg = (CtestDemoDlg*)userParam;
	if (pdlg)
	{
		pdlg->CallbackInfo(device, sex, age, id);
	}
}

void CtestDemoDlg::CallbackInfo(int device, int sex, int age, int id)
{
	CString strSex;
	CString strAge;
	if (sex == Female)
	{
		strSex = "女";
	}
	else
	{
		strSex = "男";
	}
	strAge.Format("%d", age);
	m_edtSex.SetWindowText(strSex);
	m_edtAge.SetWindowText(strAge);
}

void CtestDemoDlg::OnBnClickedBtnStart()
{
	int nDevCount = 0;
	Face_Init(&nDevCount);
	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_PIC_WND);
	Face_Create(0, pWnd->GetSafeHwnd(), OnCallbackInfo, this);
// 	Face_Create(0, NULL, OnCallbackInfo, this);
	Face_SetFaceWnd(0, ((CWnd*)GetDlgItem(IDC_PIC_FACE1))->GetSafeHwnd());
	Face_SetFaceWnd(1, ((CWnd*)GetDlgItem(IDC_PIC_FACE2))->GetSafeHwnd());
	Face_SetFaceWnd(2, ((CWnd*)GetDlgItem(IDC_PIC_FACE3))->GetSafeHwnd());
	Face_SetFaceWnd(3, ((CWnd*)GetDlgItem(IDC_PIC_FACE4))->GetSafeHwnd());
}

void CtestDemoDlg::OnActionCallbackInfo(const _TrajectoryInfo* info, void* userParam)
{
	CtestDemoDlg* pdlg = (CtestDemoDlg*)userParam;
	if (pdlg)
	{
		pdlg->ActionCallbackInfo(*info);
	}
}

void CtestDemoDlg::ActionCallbackInfo(_TrajectoryInfo info)
{
	CString strTemp;
	strTemp.Format("total:[%d],fall:[%d],punch[%d],raise[%d],id:[%d],[%d,%d]\n", info.total, info.fall, info.punch, info.raise, info.id, info.x, info.y);
	TRACE(strTemp);
}

void CtestDemoDlg::OnBnClickedBtnAction()
{
	Face_InitTrajectory();
	CWnd* pWnd = (CWnd*)GetDlgItem(IDC_PIC_WND2);
	Face_StartTrajectory(pWnd->GetSafeHwnd(), OnActionCallbackInfo, this);
}
