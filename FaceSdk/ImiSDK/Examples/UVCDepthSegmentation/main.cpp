/********************************************************************************
Copyright (c) 2015 Huajie IMI Technology Co., Ltd.
All rights reserved.

@File Name     : main.cpp
@Author        : Zhang hongliang
@Date          : 2015-06-18
@Description   : read multi depth frame
@Version       : 0.1.0
@History       :
1.2015-06-18 Zhang Hongliang Created file
********************************************************************************/

#include <stdio.h>
#include <string.h>

#include "ImiNect.h"
#include "ImiSkeleton.h"
#include "../Common/Render.h"

#ifndef _WIN32
#include <unistd.h>
#endif
#include "ImiCamera.h"


#define MAX_DEPTH				10000

#define DEPTH_IMAGE_WIDTH			640
#define DEPTH_IMAGE_HEIGHT			480 

#define DEFAULT_PERPIXEL_BITS	24
#define DEFAULT_FRAMERATE		30


#define COLOR_IMAGE_WIDTH			640
#define COLOR_IMAGE_HEIGHT			480

// window handles
SampleRender* g_pRender = NULL;

// stream handles
uint32_t		g_streamNum = 0;
ImiStreamHandle g_streams[10] = { NULL };

bool g_bColorFrameOK = false;
bool g_bDepthFrameOK = false;
bool g_bNeedFrameSync = false;

ImiCameraHandle g_cameraDevice = NULL; 

WindowHint hint( 0, 0, COLOR_IMAGE_WIDTH, COLOR_IMAGE_HEIGHT );


uint16_t tempData[1920*1440] = {0};
static RGB888Pixel g_rgbImage[COLOR_IMAGE_WIDTH * COLOR_IMAGE_HEIGHT];
static RGB888Pixel g_colorImage[COLOR_IMAGE_WIDTH * COLOR_IMAGE_HEIGHT];

static uint16_t g_depthImage[1920*1080*2];
//static uint16_t g_depthImage1080P[COLOR_IMAGE_WIDTH*COLOR_IMAGE_HEIGHT];

bool g_bSave = false;

bool g_bRotate = false;

// window callback, called by SampleRender::display()
static bool needImage(void* pData)
{
	static float	g_depthHist[MAX_DEPTH];


	uint32_t g_realColorImageSize = 0;
	uint32_t g_realDepthImageSize = 0;
	int userId[6] = {0,0,0,0,0,0};
	int userCount = 0;
	static uint64_t depth_t;
	static uint64_t color_t;

	static int g_color_width = 0;
	static int g_color_height = 0;
	static int g_depth_width = 0;
	static int g_depth_height = 0;

    // wait for stream, -1 means infinite;


	if (g_bColorFrameOK != true)
	{
        ImiCameraFrame* camFrame = NULL;

		//printf("read a color frame !!!\n");
		if (0 != imiCamReadNextFrame(g_cameraDevice, &camFrame, 100))
		{
			printf("read color frame Failed\n");
			return false;
		}

        if(NULL !=camFrame) {
            switch(camFrame->pixelFormat)
            { 
            case CAMERA_PIXEL_FORMAT_RGB888:
                {
                    memcpy((void*)&g_colorImage, (const void*)camFrame->pData, camFrame->size);
                    g_realColorImageSize = camFrame->size;

                    break;
                } 
            default: 
                {
                    break;
                }
            }
            color_t = camFrame->timeStamp;
            g_bColorFrameOK = true;
            g_color_width = camFrame->width;
            g_color_height = camFrame->height;
            imiCamReleaseFrame(&camFrame);
        }


	}
	if (g_bDepthFrameOK != true)
	{
        ImiImageFrame* pFrame = NULL;

		if (0 != imiReadNextFrame(g_streams[0], &pFrame, 100))
		{
            printf("read depth frame Failed\n");
			return false;
		}		

        memcpy(&g_depthImage, pFrame->pData, pFrame->size );


		depth_t = pFrame->timeStamp;
		g_bDepthFrameOK = true;
		g_depth_width = pFrame->width;
		g_depth_height = pFrame->height;
		imiReleaseFrame(&pFrame);
	}

    g_bSave = false;

	if(g_bColorFrameOK && g_bDepthFrameOK) 
	{
		if( g_bNeedFrameSync )
		{
			int64_t delta = (depth_t - color_t);
            delta /= 1000;

            if(delta < -20) {
                g_bDepthFrameOK = false;
                return true;
            } else if(delta > 10) {
                g_bColorFrameOK = false;
                return true;
            }

		}

        memset(g_rgbImage, 0, sizeof(g_rgbImage));
        uint16_t* pDepth = g_depthImage;

        for(int j = 0; j < COLOR_IMAGE_HEIGHT; ++j) {
            for(int i = 0; i < COLOR_IMAGE_WIDTH; ++i) {
                if((pDepth[j * COLOR_IMAGE_WIDTH + i] < 3000) && (pDepth[j * COLOR_IMAGE_WIDTH + i] != 0)){
                    g_rgbImage[j * COLOR_IMAGE_WIDTH + i] = g_colorImage[j * COLOR_IMAGE_WIDTH + i];
                }
            }
        }

		g_pRender->initViewPort();

		g_pRender->draw((uint8_t*)g_rgbImage, COLOR_IMAGE_WIDTH * COLOR_IMAGE_HEIGHT * 3, hint);

		g_pRender->update();

		g_bColorFrameOK = false;
		g_bDepthFrameOK = false;
	}
	
    return true;
}

void keyboard(unsigned char key, int32_t x, int32_t y)
{
    int32_t nRet = 0;
    uint8_t nCalibrationState = 0;

    if(key =='s') {
         g_bSave = true;

    } else if(key == 'r') {
         g_bRotate = !g_bRotate;
    } else {
        printf("Help: \n");
        printf("    's': start save \n");
        printf("    'r': rotate\n");
    }

}

int main(int argc, char** argv)
{
    if(0 != imiCamOpen(&g_cameraDevice)) {
        printf("Open UVC Camera Failed!\n");
        getchar();
        return -1;
    }

    ImiCameraFrameMode frameMode2 = {CAMERA_PIXEL_FORMAT_RGB888, COLOR_IMAGE_WIDTH, COLOR_IMAGE_HEIGHT, 30};
    if(0 != imiCamStartStream(g_cameraDevice, &frameMode2)) {
        printf("Start Camera stream Failed!\n");
        imiCamClose(g_cameraDevice);

        getchar();
        return -1;
    }

	//1.imiInitialize()
    ImiDeviceAttribute* pDeviceAttr = NULL;
    uint32_t deviceCount = 0;
    if (0 != imiInitialize()) 
	{
        printf("ImiNect Init Failed!\n");
		getchar();
        return -1;
    }
    printf("ImiNect Init Success.\n");

	//2.imiGetDeviceList()
    imiGetDeviceList(&pDeviceAttr, &deviceCount);
    if( deviceCount <= 0 || NULL == pDeviceAttr ) 
	{
        printf("Get No Connected Imidevice!");
		imiDestroy();
		getchar();
        return -1;
    }
	printf("Get %d Connected Imidevice.\n", deviceCount);

	//3.imiOpenDevice()
	ImiDeviceHandle pImiDevice = NULL;
    if( 0 != imiOpenDevice(pDeviceAttr[0].uri, &pImiDevice, 0) )
	{
        printf("Open Imidevice Failed!\n");
        goto exit;
    }
    printf("Imidevice Opened.\n");

	//A100M device sync depth and color
	g_bNeedFrameSync = true;
	//A100M device open image registration
	imiSetImageRegistration(pImiDevice, IMI_TRUE);

	////4.imiOpenStream()
	ImiFrameMode frameMode;	
	frameMode.pixelFormat = IMI_PIXEL_FORMAT_DEP_16BIT;
	frameMode.resolutionX = DEPTH_IMAGE_WIDTH;
	frameMode.resolutionY = DEPTH_IMAGE_HEIGHT;
	frameMode.bitsPerPixel = DEFAULT_PERPIXEL_BITS;
	frameMode.framerate = DEFAULT_FRAMERATE;
	imiSetFrameMode(pImiDevice, IMI_DEPTH_FRAME, &frameMode);

    if ( 0 != imiOpenStream(pImiDevice, IMI_DEPTH_FRAME, NULL, NULL, &g_streams[g_streamNum++]) )
	{
        printf("Open depth Stream Failed!\n");
        goto exit;
    }
    printf("Open depth Stream Success.\n");

    imiCamSetFramesSync(g_cameraDevice, true);

    //5.create window and set read Stream frame data callback
    g_pRender = new SampleRender("User Segmentation View", COLOR_IMAGE_WIDTH, COLOR_IMAGE_HEIGHT);  // window title & size
    g_pRender->init(argc, argv);
    g_pRender->setKeyCallback(keyboard);
    g_pRender->setDataCallback(needImage, NULL);
    g_pRender->run();

exit:

    if(g_cameraDevice != NULL) {
        imiCamSetFramesSync(g_cameraDevice, false);
        imiCamStopStream(g_cameraDevice);
        imiCamClose(g_cameraDevice);
    }

	//6.imiCloseStream()
    for (uint32_t num = 0; num < g_streamNum; ++num)
	{
        if (NULL != g_streams[num])
		{
            imiCloseStream(g_streams[num]);
        }
    }

	//7.imiCloseDevice()
    if (NULL != pImiDevice)
	{
        imiCloseDevice(pImiDevice);
    }

	//8.imiReleaseDeviceList()
    if(NULL != pDeviceAttr)
	{
        imiReleaseDeviceList(&pDeviceAttr);
    }

	//9.imiDestroy()
    imiDestroy();

    if (NULL != g_pRender)
	{
        delete g_pRender;
        g_pRender = NULL;
    }

	getchar();

    return 0;
}
