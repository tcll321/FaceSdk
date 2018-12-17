#include "stdafx.h"
#include "FaceObject.h"
#include "imiCV.h"
#include "libGenderAge_api.h"
#include "libFaceDetect.h"

CFaceObject::CFaceObject()
	:m_pDeviceAttr(NULL)
	, m_deviceCount(0)
{
}


CFaceObject::~CFaceObject()
{
}

int CFaceObject::Init()
{
	//第一步：初始化模型
	int ret;
	if (false == imi::FaceDetectionInitialization())
	{
		imi::FaceDetectionRelease();
		return -1;
	}
	ret = imi::imi_face_init_GenderAge("./model");;
	if (ret != 0)
	{
		imi::imi_face_release_GenderAge();
		return -1;
	}
	//第二步：获取图片，
	//1.imiInitialize()
	if (0 != imiInitialize())
	{
		printf("ImiNect Init Failed!\n");
		return -1;
	}
	printf("ImiNect Init Success!\n");

	//2.imiGetDiviceList()
	imiGetDeviceList(&m_pDeviceAttr, &m_deviceCount);
	if (NULL == m_pDeviceAttr)
	{
		printf("Get No Connected Imidevice!!!!!!\n");
		imiDestroy();
		return -1;
	}
	printf("Get %d Connected Imidevice.\n", m_deviceCount);
	return 0;
}

void CFaceObject::UnInit()
{
	imiDestroy();
	imi::FaceDetectionRelease();
	imi::imi_face_release_GenderAge();
}

int CFaceObject::Create(int device, HWND hwnd, Callback_OnFaceInfo info, void* userParam)
{
	ImiDeviceHandle pImiDevice = NULL;
	int i = 0;
	for (i = 0; i < m_deviceCount;i++)
	{
		if (m_pDeviceAttr[i].productId == 768)
			break;
	}
	if (0 != imiOpenDevice(m_pDeviceAttr[i].uri, &pImiDevice, IMI_DEVICE_CAMERA))
	{
		printf("Open Imidevice Failed!\n");
		return -1;
	}
	CDeviceObject *obj = new CDeviceObject();
	if (obj->CreateDev(pImiDevice, hwnd, info, userParam) != 0)
	{
		delete obj;
		return -1;
	}
	m_mapDeviceObj[device] = obj;
	return 0;
}

int CFaceObject::SetFaceWnd(int id, HWND hwnd)
{
	if (m_mapDeviceObj.size() > 0)
		m_mapDeviceObj[0]->SetFacWnd(id, hwnd);
	return 0;
}

void CFaceObject::DeleteDev(int device)
{
	MAP_DEVICEOBJECT::iterator it = m_mapDeviceObj.find(device);
	if (it != m_mapDeviceObj.end())
	{
		delete it->second;
		m_mapDeviceObj.erase(it);
	}
}