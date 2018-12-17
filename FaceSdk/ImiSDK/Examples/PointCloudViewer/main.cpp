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

#include "ImiNect.h"

#include "../Common/Render2.h"

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <fstream>
#include <time.h>

#define MAX_DEPTH				10000
#define IMAGE_WIDTH			    640
#define IMAGE_HEIGHT			480
#define DEFAULT_PERPIXEL_BITS	16
#define DEFAULT_FRAMERATE		30

// window handle
SampleRender2* g_pRender2 = NULL;

using namespace std;

bool b_Save = false;

//stream handles
uint32_t		g_streamNum = 0;
ImiStreamHandle g_streams[10] = { NULL };

ImiPoint3D g_Point3D[IMAGE_WIDTH * IMAGE_HEIGHT] = { 0 };

void calculateHistogram(float* pHistogram, int histogramSize, const ImiImageFrame* frame)
{
    const uint16_t* pDepth = (const uint16_t*)frame->pData;
    
    memset(pHistogram, 0, histogramSize*sizeof(float));
    
    int height = frame->height;
    int width = frame->width;

    unsigned int nNumberOfPoints = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x, ++pDepth)
        {
            if (*pDepth != 0)
            {
                pHistogram[*pDepth]++;
                nNumberOfPoints++;
            }
        }
    }

    for (int nIndex=1; nIndex<histogramSize; nIndex++)
    {
        pHistogram[nIndex] += pHistogram[nIndex-1];
    }

    if (nNumberOfPoints)
    {
        for (int nIndex=1; nIndex<histogramSize; nIndex++)
        {
            pHistogram[nIndex] = (256 * (1.0f - (pHistogram[nIndex] / nNumberOfPoints)));
        }
    }
}

// window callback, called by SampleRender::display()
static bool needImage(void* pData)
{
	static float		g_depthHist[MAX_DEPTH];
	static RGB888Pixel	g_rgbImage[1280 * 1024];

    int32_t avStreamIndex;

    ImiImageFrame* pFrame = NULL;

    // wait for stream, -1 means infinite;
    if(0 != imiWaitForStreams(g_streams, g_streamNum, &avStreamIndex, 100))
	{
#ifdef _WIN32
		printf("Warning:Wait for streams timeout!\n");
		Sleep(500);
#else
		usleep(500*1000);
#endif
		return false;
	}
	
    /*printf("channel index : %d\n", avStreamIndex);*/
    if ((avStreamIndex < 0)  || ((uint32_t)avStreamIndex >= g_streamNum)) 
	{
        printf("imiWaitForStream returns 0, but channel index abnormal: %d\n", avStreamIndex);
        return false;
    }

    // frame coming, read.
    if (0 != imiReadNextFrame(g_streams[avStreamIndex], &pFrame, 0))
	{
        printf("imiReadNextFrame Failed, channel index : %d\n", avStreamIndex);
        return false;
    }

	imiConvertDepthToPointCloud(pFrame, 0, 0, 0, 0, 0, &g_Point3D[0]);

	if(b_Save) {
		char filename[128] = {0};
		sprintf(filename, "./imipcd_%d.obj", time(NULL));
		ofstream fout(filename, ios::binary);

		for(int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; ++i) {
			//if(g_Point3D[i].z < 1.0 && g_Point3D[i].z > 0.1) {
				fout << "v " << g_Point3D[i].x << " " <<  g_Point3D[i].y << " " <<  g_Point3D[i].z << "\r\n";
			//}
		}
		fout.close();

		b_Save = false;
	}

	imiReleaseFrame(&pFrame);
	g_pRender2->drawMesh(&g_Point3D[0], IMAGE_WIDTH * IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT);

	return true;
}

void keyfun(unsigned char key, int32_t x, int32_t y)
{
	switch (key)
	{
	case 'S':
	case 's':
		b_Save = true;
		break;
	default:
		break;
	}
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

	//4.imiSetFrameMode()
	ImiFrameMode frameMode;
	frameMode.pixelFormat = IMI_PIXEL_FORMAT_DEP_16BIT;
	frameMode.resolutionX = IMAGE_WIDTH;
	frameMode.resolutionY = IMAGE_HEIGHT;
	frameMode.bitsPerPixel = DEFAULT_PERPIXEL_BITS;
	frameMode.framerate = DEFAULT_FRAMERATE;
	imiSetFrameMode(pImiDevice, IMI_DEPTH_FRAME, &frameMode);

	//4.imiOpenStream()
    if ( 0 != imiOpenStream(pImiDevice, IMI_DEPTH_FRAME, NULL, NULL, &g_streams[g_streamNum++]) )
	{
        printf("Open Depth Stream Failed!\n");
        goto exit;
    }
	printf("Open Depth Stream Success.\n");

    //5.create window and set read Stream frame data callback
	g_pRender2 = new SampleRender2("Point Cloud View", IMAGE_WIDTH*2, IMAGE_HEIGHT*2);  // window title & size
	g_pRender2->init(argc, argv);
	// read frame data callback
	g_pRender2->setDataCallback(needImage, NULL);
	g_pRender2->setKeyCallback(keyfun);
	g_pRender2->run();

exit:
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

    if (NULL != g_pRender2)
	{
        delete g_pRender2;
        g_pRender2 = NULL;
    }

	getchar();

    return 0;
}
