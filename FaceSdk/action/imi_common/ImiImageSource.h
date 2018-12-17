#pragma once

#include <ImiNect.h>
#include <ImiCamera.h>
#include <ImiSkeleton.h>
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#ifdef _WIN32
#pragma comment(lib, "ImiNect.lib")
#endif

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

class ImiImageSource
{

public:
	ImiImageSource():flag(0), ready(0), 
		pImiDevice(NULL), pDeviceAttr(0), g_streamNum(10), color_mode(IMI_COLOR_MODE_NV21)
	{
		memset(g_streams, 0, sizeof(ImiStreamHandle));
		//imiDestroy();
	}
	
	~ImiImageSource()
	{
		for (uint32_t num = 0; num < g_streamNum; ++num)
		{
			if (NULL != g_streams[num])
			{
				imiCloseStream(g_streams[num]);
				g_streams[num] = 0;
			}
		}

		//7.imiCloseDevice()
		if (NULL != pImiDevice)
		{
			imiCloseDevice(pImiDevice);
		}
		if (NULL != g_cameraDevice)
		{
			imiCamClose(g_cameraDevice);
		}

		//8.imiReleaseDeviceList()
		if (NULL != pDeviceAttr)
		{
			imiReleaseDeviceList(&pDeviceAttr);
		}

		imiDestroy();
	}
	int open(int channel, int flag, int mode = IMI_COLOR_MODE_NV21, bool useUVC = false)
	{
		//scj Ìí¼Ó
		//channel = 2;
		this->flag = flag;
		this->color_mode = mode;
		this->bUseUVC = useUVC;

		if (useUVC)
		{
			if (0 != imiCamOpen(&g_cameraDevice)) {
				printf("Open UVC Camera Failed!\n");
				//getchar();
				return -1;
			}
		}

		if (0 != imiInitialize())
		{
			printf("ImiNect Init Failed!\n");
			-1;
		}
		printf("ImiNect Init Success.\n");

		//2.imiGetDeviceList()
		pDeviceAttr = NULL;
		uint32_t deviceCount = 0;
		imiGetDeviceList(&pDeviceAttr, &deviceCount);
		//if (deviceCount <= channel || NULL == pDeviceAttr)
		//{
		//	printf("Get No Connected Imidevice!");
		//	imiDestroy();
		//	return -1;
		//}
		printf("Get %d Connected Imidevice.\n", deviceCount);

		//3.imiOpenDevice()
		//if (0 != imiOpenDevice(pDeviceAttr[channel].uri, &pImiDevice, IMI_DEVICE_CAMERA))
		for (int i = 0; i < deviceCount; i++)
		{
			if (pDeviceAttr[i].productId == 257)
			{
				if (0 != imiOpenDevice(pDeviceAttr[i].uri, &pImiDevice, IMI_DEVICE_CAMERA))
				{
					printf("Open Imidevice Failed!\n");
					return -1;
				}
			}
		}
		

		memset(m_index, 0, sizeof(int) * 4);
		ready = 1;
		
		
		if (flag & IMI_FLAG_COLOR)
		{
			if (!bUseUVC)
			{
				ImiFrameMode frameMode;
				frameMode.pixelFormat = color_mode ? IMI_PIXEL_FORMAT_IMAGE_RGB24 : IMI_PIXEL_FORMAT_IMAGE_YUV420SP;
				frameMode.resolutionX = IMI_COLOR_WIDTH;
				frameMode.resolutionY = IMI_COLOR_HEIGHT;
				frameMode.bitsPerPixel = DEFAULT_PERPIXEL_BITS;
				frameMode.framerate = DEFAULT_FRAMERATE;
				imiSetFrameMode(pImiDevice, IMI_COLOR_FRAME, &frameMode);

				//5.imiOpenStream()
				if (0 != imiOpenStream(pImiDevice, IMI_COLOR_FRAME, NULL, NULL, &g_streams[0]))
				{
					std::cout << "open color stream failed\n";
				}
				else
				{
					std::cout << "open color stream success\n";
				}
			}
			else
			{
				ImiCameraFrameMode frameMode = { CAMERA_PIXEL_FORMAT_RGB888, 640, 480, 30 };
				if (0 != imiCamStartStream(g_cameraDevice, &frameMode)) {
					printf("Start Camera stream Failed!\n");
					imiCamClose(g_cameraDevice);
				}
				else
				{
					std::cout << "open color stream success\n";
				}
			}		
		}
		if (flag & IMI_FLAG_DEPTH)
		{
			imiOpenStream(pImiDevice, (flag&IMI_FLAG_SKELETON)?IMI_DEPTH_SKELETON_FRAME: IMI_DEPTH_FRAME, NULL, NULL, &g_streams[1]);
		}
		if (flag & IMI_FLAG_IR)
		{
			uint8_t ld = 1;
			imiSetDeviceProperty(pImiDevice, IMI_PROPERTY_LD_OPERATE, &ld, sizeof(uint8_t));
			uint8_t flState = 1;
			int ret = imiSetDeviceProperty(pImiDevice, IMI_PROPERTY_FLOODLIGHT, &flState, sizeof(uint8_t));
			imiOpenStream(pImiDevice, 0x07, NULL, NULL, &g_streams[2]);
		}
		if (flag & IMI_FLAG_GRAVITY)
		{
			if (0 != imiOpenStream(pImiDevice, 0x08, NULL, NULL, &g_streams[3])) {
				printf("Open Acc Stream Failed!\n");
			}
		}
		int registraionMode = 1;
		imiSetDeviceProperty(pImiDevice, IMI_PROPERTY_IMAGE_REGISTRATION, &registraionMode, sizeof(registraionMode));
		return 0;
	}

	int setSize(int flag, cv::Size sz)
	{
		ImiFrameMode fMode = { IMI_PIXEL_FORMAT_DEP_16BIT, sz.width, sz.height, 16, 30 };
		if (flag == 2) {
			imiSetFrameMode(pImiDevice, IMI_DEPTH_FRAME, &fMode);
			imiCloseStream(g_streams[1]);
			imiOpenStream(pImiDevice, IMI_DEPTH_FRAME, NULL, NULL, &g_streams[1]);
		}
		else if (flag == 4)
		{
			imiSetFrameMode(pImiDevice, 0x07, &fMode);
			imiCloseStream(g_streams[2]);
			imiOpenStream(pImiDevice, 0x07, NULL, NULL, &g_streams[2]);
		}
		return 0;
	}
	bool next(cv::Mat& res, std::string& name, int ff, void* pSkFrame = 0)
	{
		//std::cout << "read frame " << ff << std::endl;
		bool bRes = true;
		if (ff == IMI_FLAG_COLOR && (IMI_FLAG_COLOR & this->flag))
		{
			if (!bUseUVC)
			{
				int32_t avStreamIndex;
				if (0 != imiWaitForStreams(g_streams + 0, 1, &avStreamIndex, 100))
				{
					return false;
				}
				ImiImageFrame* pFrame = NULL;
				if (0 != imiReadNextFrame(g_streams[0], &pFrame, 30))
				{
					bRes = false;
					printf("imiReadNext Color Frame Failed, count index : %d\n", m_index[0]);
				}
				else
				{
					if (color_mode == IMI_COLOR_MODE_NV21)
					{
						cv::Mat mColor(pFrame->height * 3 / 2, pFrame->width, CV_8UC1, pFrame->pData);
						cv::cvtColor(mColor, res, CV_YUV2BGR_NV21);
					}
					else
					{
						cv::Mat mColor(pFrame->height, pFrame->width, CV_8UC3, pFrame->pData);
						cv::cvtColor(mColor, res, CV_RGB2BGR);
					}
				}

				imiReleaseFrame(&pFrame);
			}
			else
			{
				if (!g_cameraDevice)
					return false;
				ImiCameraFrame* pCamFrame = NULL;
				if (0 != imiCamReadNextFrame(g_cameraDevice, &pCamFrame, 40)) {
					//	return false;
				}
				else
				{
					bRes = true;
					cv::Mat mColor(pCamFrame->height, pCamFrame->width, CV_8UC3, pCamFrame->pData);
					//mColor.copyTo(res);
					cv::cvtColor(mColor, res, CV_RGB2BGR);
				}
				imiCamReleaseFrame(&pCamFrame);
			}
			name = std::to_string(m_index[0]) + "_c";
			m_index[0]++;
		}
		else if (ff == IMI_FLAG_DEPTH && (this->flag & IMI_FLAG_DEPTH))
		{
			int32_t avStreamIndex;
			if (0 != imiWaitForStreams(g_streams + 1, 1, &avStreamIndex, 100))
			{
				return false;
			}
			ImiImageFrame* pFrame = NULL;
			if (0 != imiReadNextFrame(g_streams[1], &pFrame, 30))
			{
				bRes = false;
				printf("imiReadNext Depth Frame Failed, channel index : %d\n", m_index[1]);
			}
			else
			{
				cv::Mat mColor(pFrame->height, pFrame->width, CV_16UC1, pFrame->pData);
				mColor.copyTo(res);

				if (pSkFrame && (this->flag & IMI_FLAG_SKELETON))
				{
					ImiSkeletonFrame* p = (ImiSkeletonFrame*)pSkFrame;
					
					if(pFrame->pSkeletonData)
					{
						ImiSkeletonFrame* pS = (ImiSkeletonFrame*)pFrame->pSkeletonData;
						for (int i = 0; i < 6; i++)
						{
							p->skeletonData[i].trackingState = pS->skeletonData[i].trackingState;
							p->skeletonData[i].trackingID = pS->skeletonData[i].trackingID;
							for (int k = 0; k < 20; k++)
							{
								p->skeletonData[i].skeletonPositions[k] = pS->skeletonData[i].skeletonPositions[k];
								p->skeletonData[i].skeletonPositionTrackingState[k] = pS->skeletonData[i].skeletonPositionTrackingState[k];
							}
						}
					}
					else
					{
						memset(p->skeletonData, 0, sizeof(ImiSkeletonData) * 6);
					}
				}
			}
			imiReleaseFrame(&pFrame);
			name = std::to_string(m_index[1]) + "_d";
			m_index[1]++;
		}
		else if (ff & IMI_FLAG_IR && (this->flag & IMI_FLAG_IR))
		{
			int32_t avStreamIndex;
			if (0 != imiWaitForStreams(g_streams + 2, 1, &avStreamIndex, 100))
			{
				return false;
			}
			ImiImageFrame* pFrame = NULL;
			if (0 != imiReadNextFrame(g_streams[2], &pFrame, 30))
			{
				bRes = false;
				printf("imiReadNext IR Frame Failed, channel index : %d\n", m_index[2]);

			}
			else
			{
				cv::Mat mColor(pFrame->height, pFrame->width, CV_16UC1, pFrame->pData);
				mColor.copyTo(res);
			}
			imiReleaseFrame(&pFrame);
			name = std::to_string(m_index[2]) + "_i";
			m_index[2]++;
		}
		else if ((ff == IMI_FLAG_GRAVITY) && (this->flag & IMI_FLAG_GRAVITY))
		{
			int32_t avStreamIndex;
			if (0 != imiWaitForStreams(g_streams + 3, 1, &avStreamIndex, 100))
			{
				return false;
			}
			ImiImageFrame* pFrame = NULL;
			if (0 != imiReadNextFrame(g_streams[3], &pFrame, 30))
			{
				printf("imiReadNext Gravity Failed, channel index : %d\n", m_index[2]);
			}
			else
			{
				bRes = true;
				cv::Mat mGrav(1, pFrame->size, CV_8UC1, pFrame->pData);
				mGrav.copyTo(res);
			}
			imiReleaseFrame(&pFrame);
			name = std::to_string(m_index[3]) + "_g";
			m_index[3]++;
		}
		else
		{
			bRes = false;
		}
		// frame coming, read.
		return bRes;
	};


	int m_index[4];
	int flag;
	int ready;
	int color_mode;
	ImiDeviceHandle pImiDevice;
	ImiStreamHandle g_streams[10];
	uint32_t		g_streamNum;
	ImiDeviceAttribute* pDeviceAttr;
	bool bUseUVC = false;
	ImiCameraHandle g_cameraDevice = NULL;
};