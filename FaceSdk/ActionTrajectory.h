#pragma once
#include <memory>
#include "SyncCameraManager.h"
#include "FaceSdk.h"

class CActionTrajectory
{
public:
	CActionTrajectory();
	~CActionTrajectory();

	int InitTracker();
	int Start(HWND hwnd, Callback_OnTrajectoryInfo info, void* userParam);

	static DWORD WINAPI ThreadProFunc(LPVOID lpParam);
	void ProFunc();

private:
	std::shared_ptr<SyncShelfCameraManager> m_pCameraManager;
	void* m_pHumanTracker = 0;
	HWND m_hwindow;

	HANDLE m_hThread;
	DWORD m_dwThreadId;
	bool	m_bIsRun;

	Callback_OnTrajectoryInfo m_callback;
	void*	m_userParam;
};

