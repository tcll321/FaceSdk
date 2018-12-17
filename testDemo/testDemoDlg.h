
// testDemoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "FaceSdk.h"

// CtestDemoDlg 对话框
class CtestDemoDlg : public CDialogEx
{
// 构造
public:
	CtestDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTDEMO_DIALOG };

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
	afx_msg void OnBnClickedBtnStart();

	static void OnCallbackInfo(int device, int sex, int age, int id, void* userParam);
	void CallbackInfo(int device, int sex, int age, int id);

	static void OnActionCallbackInfo(const _TrajectoryInfo* info, void* userParam);
	void ActionCallbackInfo(_TrajectoryInfo info);

	CEdit m_edtSex;
	CEdit m_edtAge;
	afx_msg void OnBnClickedBtnAction();
};
