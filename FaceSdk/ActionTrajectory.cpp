#include "stdafx.h"
#include "ActionTrajectory.h"
#include "Config/PathConfig.h"
#include "Config/RoomConfig.h"
#include "HumanTrackerInterface.h"

CActionTrajectory::CActionTrajectory()
	:m_bIsRun(false)
	, m_dwThreadId(0)
	, m_hThread(NULL)
	, m_hwindow(NULL)
{
}


CActionTrajectory::~CActionTrajectory()
{
	if (m_bIsRun)
	{
		m_bIsRun = false;
		CloseHandle(m_hThread);
		m_dwThreadId = 0;
	}
	ReleaseHumanTracker(m_pHumanTracker);
}

int CActionTrajectory::InitTracker()
{
	PathConfig pathConfig;
	std::string szPathConfig = "./path_config.txt";
	pathConfig.load(szPathConfig.c_str());

	RoomConfig roomConfig;
	ShelfGroupConfig shelfConfig;

	DepthCameraGroupConfig depthConfig;
	std::string szCamera = pathConfig.getPath("depth_camera_tracker");
	depthConfig.load(szCamera.c_str());
	m_pCameraManager = std::make_shared<SyncShelfCameraManager>();
	for (int i = 0; i < depthConfig.vConfig.size(); i++)
	{
		m_pCameraManager->_add_depth_camera(depthConfig.vConfig[i]);
	}

	m_pCameraManager->start();

	m_pHumanTracker = CreateHumanTracker(szPathConfig.c_str());
	return 0;
}

DWORD WINAPI CActionTrajectory::ThreadProFunc(LPVOID lpParam)
{
	CActionTrajectory* obj = (CActionTrajectory*)lpParam;
	if (obj)
	{
		obj->ProFunc();
	}
	return 0;
}
void CActionTrajectory::ProFunc()
{
	cv::Mat color_res;
	RECT rt;
	cvNamedWindow("action", CV_WINDOW_AUTOSIZE);
	HWND mHWnd = (HWND)cvGetWindowHandle("action");
	HWND mHParent = ::GetParent(mHWnd);
	::GetWindowRect(m_hwindow, &rt);
	::SetParent(mHWnd, m_hwindow);
	::ShowWindow(mHParent, SW_HIDE);
	cv::Size outSize;
	outSize.width = rt.right - rt.left;
	outSize.height = rt.bottom - rt.top;
	while (m_bIsRun)
	{
		std::vector<cv::Mat> vColor, vDepth, vSkeleton;
		if (m_pCameraManager->fetch(vColor, vDepth, &vSkeleton))
		{
			bool bFine = true;
			for (int i = 0; i < vDepth.size(); i++)
			{
				if (vDepth[i].empty())
				{
					bFine = false;
					break;
				}
			}
			if (!bFine)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				continue;
			}

			int nImageCount = vDepth.size();
			shop_image_t* vImage = new shop_image_t[nImageCount];
			for (int i = 0; i < vDepth.size(); i++)
			{
				vImage[i].w = vDepth[i].cols;
				vImage[i].h = vDepth[i].rows;
				vImage[i].data = vDepth[i].data;
				vImage[i].c = vDepth[i].channels();
				vImage[i].step = vDepth[i].step[0];
				vImage[i].esz = vDepth[i].elemSize();
			}
			cv::resize(vDepth[0], color_res, outSize, 0, 0, cv::INTER_AREA);
			ImiSkeletonFrame* pSk = vSkeleton.empty() ? 0 : new ImiSkeletonFrame[vSkeleton.size()];
			for (int i = 0; i < vSkeleton.size(); i++)
			{
				if (!vSkeleton[i].empty())
					memcpy(pSk + i, vSkeleton[i].data, sizeof(ImiSkeletonFrame));
			}
			tracker_t vTracker[100];
			int nTracker = 100;
			tracker_stat_t stat;
			ProcessHumanTrackerSync(m_pHumanTracker, vImage, 2, pSk, vSkeleton.size(), vTracker, nTracker, &stat);//开始跟踪------重点，在此，可以把获取年龄性别的接口放这

			if (m_callback)
			{
				_TrajectoryInfo info;
				info.total = stat.total < 0 ? 0 : stat.total;
				info.fall = stat.fall < 0 ? 0 : stat.fall;
				info.punch = stat.punch < 0 ? 0 : stat.punch;
				info.total = stat.raise < 0 ? 0 : stat.raise;
				info.nTracker = nTracker;
				for (int i = 0; i < nTracker;i++)
				{
					info.id = vTracker[i].id;
					info.x = vTracker[i].pos.x;
					info.y = vTracker[i].pos.y;
					m_callback(&info, m_userParam);
				}
			}
			cv::imshow("action", color_res);
			cv::waitKey(1);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
}

int CActionTrajectory::Start(HWND hwnd, Callback_OnTrajectoryInfo info, void* userParam)
{
	m_hwindow = hwnd;
	m_callback = info;
	m_userParam = userParam;
	m_bIsRun = true;
	m_hThread = CreateThread(NULL	// 默认安全属性
		, NULL		// 默认堆栈大小
		, ThreadProFunc // 线程入口地址
		, this	//传递给线程函数的参数
		, 0		// 指定线程立即运行
		, &m_dwThreadId	//线程ID号
		);
	return 0;
}
