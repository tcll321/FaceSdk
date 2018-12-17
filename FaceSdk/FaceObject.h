#pragma once
#include <map>
#include "ImiNect.h"
#include "ImiCameraDefines.h"
#include "ImiCamera.h"
#include "DeviceObject.h"

class CFaceObject
{
public:
	CFaceObject();
	~CFaceObject();

	int Init();
	void UnInit();
	int GetDeviceCount() { return m_deviceCount; };

	int Create(int device, HWND hwnd, Callback_OnFaceInfo info, void* userParam);
	int SetFaceWnd(int id, HWND hwnd);
	void DeleteDev(int device);

private:
	ImiDeviceAttribute* m_pDeviceAttr = NULL;
	uint32_t m_deviceCount = 0;

	typedef std::map<int, CDeviceObject*> MAP_DEVICEOBJECT;
	MAP_DEVICEOBJECT	m_mapDeviceObj;
};

