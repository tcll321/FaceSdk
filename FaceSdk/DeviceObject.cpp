#include "stdafx.h"
#include <string>
#include "DeviceObject.h"
#include "libGenderAge_api.h"
#include "libFaceDetect.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"

using namespace std;

#define DISPLAY_SIZE_1	2
#define DISPLAY_SIZE_2	1.75

#define IMI_FLAG_COLOR 1
#define IMI_FLAG_DEPTH 2
#define IMI_FLAG_IR 4
#define IMI_FLAG_GRAVITY 8
#define IMI_FLAG_SKELETON 16

#define IMI_COLOR_WIDTH 640
#define IMI_COLOR_HEIGHT 480
#define IMI_DEPTH_WIDTH 640
#define IMI_DEPTH_HEIGHT 480

#define DEFAULT_PERPIXEL_BITS	16
#define DEFAULT_FRAMERATE		30

#define IMI_COLOR_MODE_NV21 0
#define IMI_COLOR_MODE_RGB24 1

#define IMAGE_WIDTH			    1920//640
#define IMAGE_HEIGHT			1080//480

#define DEPTH_WIDTH			    640
#define DEPTH_HEIGHT			480

CDeviceObject::CDeviceObject()
	:m_bIsRun(false)
	, m_bUseUVC(false)
	, m_dwThreadId(0)
	, m_hThread(NULL)
	, m_color_mode(0)
	, m_hWnd(NULL)
	, m_pImiDevice(NULL)
	, m_pCameraDevice(NULL)
{
}


CDeviceObject::~CDeviceObject()
{
	if (m_bIsRun)
	{
		m_bIsRun = false;
		CloseHandle(m_hThread);
		m_dwThreadId = 0;
	}
}

int CDeviceObject::CreateDev(ImiDeviceHandle handle, HWND hwnd, Callback_OnFaceInfo info, void* userParam)
{
// 	if (hwnd == NULL)
// 		return -1;
	m_hWnd = hwnd;
	m_callback = info;
	m_userParam = userParam;
	m_pImiDevice = handle;
	ImiCameraHandle g_cameraDevice = NULL;
	int flag = 27;
	if (IMI_FLAG_COLOR)
	{
		if (!m_bUseUVC)
		{
			ImiFrameMode frameMode;
			frameMode.pixelFormat = IMI_PIXEL_FORMAT_DEP_16BIT;// m_color_mode ? IMI_PIXEL_FORMAT_IMAGE_RGB24 : IMI_PIXEL_FORMAT_IMAGE_YUV420SP;
			frameMode.resolutionX = IMI_COLOR_WIDTH;
			frameMode.resolutionY = IMI_COLOR_HEIGHT;
			frameMode.bitsPerPixel = DEFAULT_PERPIXEL_BITS;
			frameMode.framerate = DEFAULT_FRAMERATE;
			imiSetFrameMode(m_pImiDevice, IMI_DEPTH_FRAME, &frameMode);
			//4.imiOpenStream()
			if (0 != imiOpenStream(m_pImiDevice, IMI_DEPTH_FRAME, NULL, NULL, &m_streams[0]))
			{
				std::cout << "open color stream failed\n";
			}
			else
			{
				std::cout << "open color stream success\n";
			}
		}
	}

	//4.imiOpenStream()
// 	if (flag & IMI_FLAG_DEPTH)
// 	{
// 		imiOpenStream(m_pImiDevice, (flag&IMI_FLAG_SKELETON) ? IMI_DEPTH_SKELETON_FRAME : IMI_DEPTH_FRAME, NULL, NULL, &m_streams[1]);
// 	}
// 	int registraionMode = 1;
// 	imiSetDeviceProperty(m_pImiDevice, IMI_PROPERTY_IMAGE_REGISTRATION, &registraionMode, sizeof(registraionMode));

	// open UVC camera
	if (0 != imiCamOpen(&m_pCameraDevice)) {		//a200_sdk-V1.6.5 add
		printf("Open UVC Camera Failed!\n");
		return -1;
	}
	ImiCameraFrameMode CameraFrameMode = { CAMERA_PIXEL_FORMAT_RGB888, IMAGE_WIDTH, IMAGE_HEIGHT, 30 };
	int ret = imiCamStartStream(m_pCameraDevice, &CameraFrameMode);			//a200_sdk-V1.6.5 revise
	if (0 != ret)
	{
		std::printf("Open UVC Color Stream Failed!ret=%d\n", ret);
	}
	else{
		std::printf("Open UVC Color Stream Success.\n");
	}

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

DWORD WINAPI CDeviceObject::ThreadProFunc(LPVOID lpParam)
{
	CDeviceObject* obj = (CDeviceObject*)lpParam;
	if (obj)
	{
		obj->ProFunc();
	}
	return 0;
}

void CDeviceObject::ProFunc()
{
	cv::Mat color_res(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);
	cv::Size outSizeWnd;
	if (m_hWnd)
	{
		RECT rt;
		cvNamedWindow("result", CV_WINDOW_NORMAL);
// 		HWND mHWnd = (HWND)cvGetWindowHandle("result");
// 		HWND mHParent = ::GetParent(mHWnd);
// 		::SetParent(mHWnd, m_hWnd);
// 		::ShowWindow(mHParent, SW_HIDE);
		::GetWindowRect(m_hWnd, &rt);
		outSizeWnd.width = rt.right - rt.left;
		outSizeWnd.height = rt.bottom - rt.top;
	}
	while (m_bIsRun)
	{
		bool bRes = true;
		ImiCameraFrame* pFrame = NULL;
		//彩色图
		if (0 != imiCamReadNextFrame(m_pCameraDevice, &pFrame, 40))
		{
			bRes = false;
			printf("----------------------imiReadNext Color Frame Failed, count index-----------------------------\n");
		}
		else
		{
			uchar *pBuff = (uchar*)pFrame->pData;
			for (int h = 0; h < IMAGE_HEIGHT; h++)
			{
				uchar *ptr_color = color_res.ptr<uchar>(h);  //第i行的指针 
				for (int w = 0; w < IMAGE_WIDTH; w++)
				{
					ptr_color[3 * w + 2] = pBuff[3 * w + 3 * h*IMAGE_WIDTH];
					ptr_color[3 * w + 1] = pBuff[3 * w + 1 + 3 * h*IMAGE_WIDTH];
					ptr_color[3 * w] = pBuff[3 * w + 2 + 3 * h*IMAGE_WIDTH];
				}
			}
			imiCamReleaseFrame(&pFrame);		//a200_sdk-V1.6.5 revise
		}//for
		//如果彩色和深度都获取成功
		if (bRes == true && color_res.data!=NULL)
		{
			//开始跟踪
			imi::imiMat in_image(color_res.rows, color_res.cols, IMI_CV_8UC3, color_res.data);
			std::vector<imi::FaceInfo> faces = detectFace(&in_image);
			/**********计算性别年龄**********/
			int ret = imi::imi_face_get_GenderAge_refine(&in_image, faces);//基于视频的demo
			if (ret != 0)
			{
				printf("imi_face_get_feature failed.(ret=%d)\n", ret);
			}
			else
			{
				/****************结果显示******************/
				for (int i = 0; i < faces.size(); i++)
				{
					_FaceInfo info = {0};
					cv::Rect bbox(faces[i].bbox.x, faces[i].bbox.y, faces[i].bbox.width, faces[i].bbox.height);
					cv::rectangle(color_res, bbox, cv::Scalar(255));
					if (0 == faces[i].gender)
					{
						info.sex = 0;
						putText(color_res, "Female", cv::Point(faces[i].bbox.x + faces[i].bbox.width, faces[i].bbox.y), DISPLAY_SIZE_1, DISPLAY_SIZE_2, CV_RGB(3, 235, 247), 2, 8);
					}
					else if (1 == faces[i].gender)
					{
						info.sex = 1;
						putText(color_res, "Male", cv::Point(faces[i].bbox.x + faces[i].bbox.width, faces[i].bbox.y), DISPLAY_SIZE_1, DISPLAY_SIZE_2, CV_RGB(3, 235, 247), 2, 8);
					}
					if (faces[i].age != -1)
					{
						info.age = faces[i].age;
						putText(color_res, "Age: " + to_string(faces[i].age), cv::Point(faces[i].bbox.x + faces[i].bbox.width, faces[i].bbox.y + 40), DISPLAY_SIZE_1, DISPLAY_SIZE_2, CV_RGB(3, 235, 247), 2, 8);
					}
					if (m_callback && info.age>0)
					{
// 						if (m_mapFaceWnd.size()>i)
// 						{
// 							cv::Mat face_mat;
// 							cv::Size outSize;
// 							outSize.width = m_mapFaceWnd[i].width;
// 							outSize.height = m_mapFaceWnd[i].height;
// 							resize(color_res(bbox), face_mat, outSize, 0, 0, cv::INTER_LINEAR);
// 							cv::imshow(m_mapFaceWnd[i].wndName, face_mat);
// 							cv::waitKey(1);
// 						}
						m_callback(0, info.sex, info.age, 0, m_userParam);
					}
				}
			}
			if (m_hWnd)
			{
				cv::Mat window_mat;
				cv::resize(color_res, window_mat, outSizeWnd, 0, 0, cv::INTER_LINEAR);
				cv::imshow("result", window_mat);
				cv::waitKey(1);
			}
		}//bRes
	}//while(1)
}

int CDeviceObject::SetFacWnd(int id, HWND hwnd)
{
	char szWndName[20] = { 0 };
	sprintf_s(szWndName, "faceWnd_%d", id);
	cvNamedWindow(szWndName, CV_WINDOW_NORMAL);
	HWND mHWnd = (HWND)cvGetWindowHandle(szWndName);
	HWND mHParent = ::GetParent(mHWnd);
	RECT rt;
	::GetWindowRect(hwnd, &rt);
	::SetParent(mHWnd, hwnd);
	::ShowWindow(mHParent, SW_HIDE);
	WndInfo info;
	info.width = rt.right - rt.left;
	info.height = rt.bottom - rt.top;
	info.wndName = szWndName;
	m_mapFaceWnd[id] = info;
	return 0;
}