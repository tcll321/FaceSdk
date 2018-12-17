// FaceSdk.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "FaceSdk.h"
#include "FaceObject.h"
#include "ActionTrajectory.h"

CFaceObject *g_faceObj = NULL;
CActionTrajectory *g_actTraject = NULL;

// 这是导出函数的一个示例。
FACESDK_API int Face_Init(int *deviceCnt)
{
	if (g_faceObj)
		delete g_faceObj;
	int nRet = 0;
	g_faceObj = new CFaceObject();
	if (g_faceObj == NULL)
		return -1;
	nRet = g_faceObj->Init();
	if (nRet != 0)
		return -1;

	*deviceCnt = g_faceObj->GetDeviceCount();

	return 0;
}

FACESDK_API int Face_Create(int device, HWND hwnd, Callback_OnFaceInfo info, void* userParam)
{
	if (g_faceObj == NULL)
		return -1;
	g_faceObj->Create(device, hwnd, info, userParam);
}

FACESDK_API int Face_SetFaceWnd(int faceId, HWND hwnd)
{
	if (g_faceObj == NULL)
		return -1;
	g_faceObj->SetFaceWnd(faceId, hwnd);
}

FACESDK_API int Face_Delete(int device)
{
	if (g_faceObj == NULL)
		return -1;
	g_faceObj->DeleteDev(device);

}

FACESDK_API int Face_InitTrajectory()
{
	if (g_actTraject)
		delete g_actTraject;

	g_actTraject = new CActionTrajectory();
	return g_actTraject->InitTracker();
}

FACESDK_API int Face_StartTrajectory(HWND hwnd, Callback_OnTrajectoryInfo info, void* userParam)
{
	if (g_actTraject == NULL)
	{
		return -1;
	}
	return g_actTraject->Start(hwnd, info, userParam);
}

FACESDK_API void Face_StopTrajectory()
{
	if (g_actTraject)
	{
		delete g_actTraject;
		g_actTraject = NULL;
	}
}
