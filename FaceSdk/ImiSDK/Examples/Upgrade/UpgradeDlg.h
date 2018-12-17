
// UpgradeDlg.h : 头文件
//

#pragma once
#include "ImiDefines.h"
#include "ImiUpgrade.h"
#include "afxwin.h"
#include "afxcmn.h"


enum ImiMsgType
{
	IMI_MSG_INVALID               = -1,

    IMI_MSG_SHOW_MESSAGEBOX       = 0,

};

// CUpgradeDlg 对话框
class CUpgradeDlg : public CDialogEx
{
// 构造
public:
	CUpgradeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_UPGRADE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnClose();
    afx_msg LRESULT onProcUserMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:

	void checkUpdate();

    void upgradeStepPrompt(IMI_UPGRADE_PROMPT promptID, float progress, IMI_UPGRADE_ERRCODE errCode, const char* errMsg);

	static void _stdcall ImiDeviceUpgradeStepPromptUser(IMI_UPGRADE_PROMPT promptID, float progress, IMI_UPGRADE_ERRCODE errCode, const char* errMsg, void* pUserData);

    static VOID CALLBACK onTimer(HWND hwnd, UINT nMsg, UINT_PTR nTimerid, DWORD dwTime);

private:

    ImiDeviceHandle         m_pImiDevice;

	ImiUpgradeCallbacks    m_upgradecallBacks;

    CListBox                m_listBox;
    CProgressCtrl         m_upgradeProgressCtrl;
    CStatic                 m_upgradeProgressValue;
public:
	CStatic m_StaticMsg;
	CStatic m_StaticVersion;
};
