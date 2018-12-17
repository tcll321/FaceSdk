/********************************************************************************
  Copyright (c) 2015 Huajie IMI Technology Co., Ltd.
  All rights reserved.

  @File Name     : main.cpp
  @Author        : Zhang hongliang
  @Date          : 2015-06-18
  @Description   : read color frame
  @Version       : 0.1.0
  @History       :
  1.2015-06-18 Zhang Hongliang Created file
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "ImiNect.h"

#include "../Common/Render.h"

#define IMAGE_WIDTH			640
#define IMAGE_HEIGHT			480
#define DEFAULT_PERPIXEL_BITS	16
#define DEFAULT_FRAMERATE		30

// window handle
SampleRender*	g_pRender = NULL;

// stream handles
ImiStreamHandle g_streams[10];
uint32_t		g_streamNum = 0;

// window callback, called by SampleRender::display()
static bool needImage(void* pData)
{
	static RGB888Pixel	g_rgbImage[1280 * 1024];

    int32_t avStreamIndex;

    // wait for stream, -1 means infinite;
    if(0 != imiWaitForStreams(g_streams, g_streamNum, &avStreamIndex, 100))
	{
		return false;
	}

    // frame coming, read.
	ImiImageFrame* imiFrame = NULL;
    if(0 != imiReadNextFrame(g_streams[avStreamIndex], &imiFrame, 0))
	{
		return false;
	}

    // show to the window
    if (IMI_COLOR_FRAME == imiFrame->type)
	{
		switch(imiFrame->pixelFormat)
		{
			case IMI_PIXEL_FORMAT_IMAGE_RGB24:
			{
				g_pRender->draw(-1, (uint8_t*)imiFrame->pData, imiFrame->size, imiFrame->width, imiFrame->height);
				break;
			}					
			case IMI_PIXEL_FORMAT_IMAGE_YUV420SP:
			{
				YUV420SPToRGB((uint8_t*)g_rgbImage, (uint8_t*)imiFrame->pData, imiFrame->width, imiFrame->height);
				g_pRender->draw(-1, (uint8_t*)g_rgbImage, imiFrame->width * imiFrame->height * 3, imiFrame->width, imiFrame->height);
				break;
			}
			case IMI_PIXEL_FORMAT_IMAGE_YUV422:
			{
				YUV422ToRGB((uint8_t*)g_rgbImage, (uint8_t*)imiFrame->pData, imiFrame->width, imiFrame->height);
				g_pRender->draw(-1, (uint8_t*)g_rgbImage, imiFrame->width * imiFrame->height * 3, imiFrame->width, imiFrame->height);
				break;
			}					
		}
    }

    // call this while imiReadNextFrame returns 0;
    imiReleaseFrame(&imiFrame);
    return true;
}

int main(int argc, char** argv)
{
	//1.imiInitialize()
    if (0 != imiInitialize()) 
	{
        printf("ImiNect Init Failed!\n");
		getchar();
        return -1;
    }
	printf("ImiNect Init Success.\n");
	
	//2.imiGetDeviceList()
	ImiDeviceAttribute* pDeviceAttr = NULL;
	uint32_t deviceCount = 0;
    imiGetDeviceList(&pDeviceAttr, &deviceCount);
    if((deviceCount <= 0) || (NULL == pDeviceAttr)) 
	{
        printf("Get No Connected Imidevice!");
        imiDestroy();
		getchar();
		return -1;
    }
	printf("Get %d Connected Imidevice.\n", deviceCount);

	//3.imiOpenDevice()
	ImiDeviceHandle  pImiDevice = NULL;
    if (0 != imiOpenDevice(pDeviceAttr[0].uri, &pImiDevice, 0))
	{
        printf("Open Imidevice Failed!\n");
        goto exit;
    }
	printf("Imidevice Opened.\n");

	//4.imiSetFrameMode()
	ImiFrameMode frameMode;
	frameMode.pixelFormat = IMI_PIXEL_FORMAT_IMAGE_RGB24;
	frameMode.resolutionX = IMAGE_WIDTH;
	frameMode.resolutionY = IMAGE_HEIGHT;
	frameMode.bitsPerPixel = DEFAULT_PERPIXEL_BITS;
	frameMode.framerate = DEFAULT_FRAMERATE;
	imiSetFrameMode(pImiDevice, IMI_COLOR_FRAME, &frameMode);

	//5.imiOpenStream()
    if (0 != imiOpenStream(pImiDevice, IMI_COLOR_FRAME, NULL, NULL, &g_streams[g_streamNum++]))
	{
        printf("Open Color Stream Failed!\n");
        goto exit;
    }
	printf("Open Color Stream Success.\n");

    //6.create window and set read Stream frame data callback
    g_pRender = new SampleRender("RGB Color View", IMAGE_WIDTH, IMAGE_HEIGHT);  // window title & size
    g_pRender->init(argc, argv);
    g_pRender->setDataCallback(needImage, NULL);
    g_pRender->run();

exit:
	//7.imiCloseStream()
    for (uint32_t num = 0; num < g_streamNum; ++num)
	{
        if (NULL != g_streams[num])
		{
            imiCloseStream(g_streams[num]);
        }
    }

	//8.imiCloseDevice()
    if (NULL != pImiDevice)
	{
        imiCloseDevice(pImiDevice);
    }

	//9.imiReleaseDeviceList()
    if(NULL != pDeviceAttr)
	{
        imiReleaseDeviceList(&pDeviceAttr);
    }

	//10.imiDestroy()
    imiDestroy();

    if (NULL != g_pRender)
	{
        delete g_pRender;
        g_pRender = NULL;
    }

	getchar();

    return 0;

}
