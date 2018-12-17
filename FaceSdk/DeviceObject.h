#pragma once
#include <map>
#include "imiCV.h"
#include "ImiNect.h"
#include "ImiCameraDefines.h"
#include "ImiCamera.h"
#include "define.h"
#include "FaceSdk.h"

class CDeviceObject
{
public:
	CDeviceObject();
	~CDeviceObject();

	int CreateDev(ImiDeviceHandle handle, HWND hwnd, Callback_OnFaceInfo info, void* userParam);
	int SetFacWnd(int id, HWND hwnd);

	static DWORD WINAPI ThreadProFunc(LPVOID lpParam);
	void ProFunc();

private:
	ImiDeviceHandle m_pImiDevice;
	ImiCameraHandle m_pCameraDevice;
	ImiStreamHandle m_streams[10];
	uint32_t		m_streamNum;
	bool m_bUseUVC;
	int		m_color_mode;
	HWND	m_hWnd;

	HANDLE m_hThread;
	DWORD m_dwThreadId;
	bool	m_bIsRun;

	Callback_OnFaceInfo m_callback;
	void*	m_userParam;

	typedef struct WndInfoStruct
	{
		std::string wndName;
		int width;
		int height;
	}WndInfo;
	typedef std::map<int, WndInfo>	MAP_FACEWND;
	MAP_FACEWND m_mapFaceWnd;
};

