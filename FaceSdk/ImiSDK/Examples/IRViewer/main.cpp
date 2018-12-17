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

#include "../Common/Render.h"

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#define MAX_DEPTH				10000
#define IMAGE_WIDTH			    640
#define IMAGE_HEIGHT			480
#define DEFAULT_PERPIXEL_BITS	16
#define DEFAULT_FRAMERATE		30

typedef struct {  
    unsigned long    bfType;  
    unsigned long   bfSize;  
    unsigned short    bfReserved1;  
    unsigned short    bfReserved2;  
    unsigned long   bfOffBits;  
} BMPFILEHEADER_T;  

typedef struct{  
    unsigned long      biSize;  
    long       biWidth;  
    long       biHeight;  
    unsigned short       biPlanes;  
    unsigned short       biBitCount;  
    unsigned long      biCompression;  
    unsigned long      biSizeImage;  
    long       biXPelsPerMeter;  
    long       biYPelsPerMeter;  
    unsigned long      biClrUsed;  
    unsigned long      biClrImportant;  
} BMPINFOHEADER_T; 

bool bSave = false;
uint8_t g_bmpColor[1920*1080*3] = {0};


// window handle
SampleRender* g_pRender = NULL;

//stream handles
uint32_t		g_streamNum = 0;
ImiStreamHandle g_streams[10] = { NULL };

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

void keyboard(unsigned char key, int32_t x, int32_t y)
{
    int32_t nRet = 0;
    uint8_t nCalibrationState = 0;

    if(key =='s') {
        bSave = true;

    } else {
        printf("Please input 's'  to save image: \n");
    }

}

int32_t takePhoto(const char* bmpImagePath, const uint8_t* pframe, int width, int height)
{
    BMPFILEHEADER_T	bmfh;			// bitmap file header
    BMPINFOHEADER_T	bmih;			// bitmap info header (windows)

    const int OffBits = 54;

    int32_t imagePixSize = width * height;

    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfType = 0x4d42;
    bmfh.bfOffBits = OffBits;		// 头部信息54字节
    bmfh.bfSize = imagePixSize * 3 + OffBits;

    memset(&bmih, 0, sizeof(BMPINFOHEADER_T));
    bmih.biSize = 40;				// 结构体大小为40
    bmih.biPlanes = 1;
    bmih.biSizeImage = imagePixSize * 3;

    bmih.biBitCount = 24;
    bmih.biCompression = 0;
    bmih.biWidth = width;
    bmih.biHeight = -height;


    memcpy( (void*)g_bmpColor, pframe, imagePixSize*3);

    //rgb->bgr
    for( int i = 0; i < imagePixSize; ++i )
    {
        char r = g_bmpColor[3*i + 2];
        g_bmpColor[3*i+2] = g_bmpColor[3*i];
        g_bmpColor[3*i] = r;
    }

    char buf[128]= {0};
    std::string fullPath = bmpImagePath;


    FILE* pSaveBmp = fopen(fullPath.c_str(), "wb");
    if( NULL == pSaveBmp)
    {
        return -1;
    }

    fwrite( &bmfh, 8, 1,  pSaveBmp );
    fwrite( &bmfh.bfReserved2, sizeof(bmfh.bfReserved2), 1, pSaveBmp);  
    fwrite( &bmfh.bfOffBits, sizeof(bmfh.bfOffBits), 1, pSaveBmp);  
    fwrite( &bmih, sizeof(BMPINFOHEADER_T), 1, pSaveBmp );  
    fwrite( g_bmpColor, imagePixSize*3, 1, pSaveBmp);

    fclose(pSaveBmp);

    return 0;
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

    uint32_t rgbSize;
    uint16_t * pde = (uint16_t*)pFrame->pData;

    static uint16_t tempIr[640*480] = {0};

    memset(tempIr, 0, 640 * 480 * 2);
    //for (int i = 0; i < 396; i++)
    //{
    //    for (int j = 0; j < 636; j++)
    //    {
    //        tempIr[i * 640 + j] = pde[(i + 4) * 640 + (j + 4)];
    //    }
    //}


    memcpy(tempIr, pFrame->pData, pFrame->size);

    if(bSave) {
        FILE* pFile = fopen("IR_reg.raw", "wb");

        fwrite(tempIr, pFrame->size, 1, pFile);

        fclose(pFile);

        takePhoto("IR_reg.bmp", (const uint8_t*)&g_rgbImage, pFrame->width, pFrame->height);

        bSave = false;
    }

    for (rgbSize = 0; rgbSize < pFrame->size/2; ++rgbSize) 
    {
        g_rgbImage[rgbSize].r = tempIr[rgbSize] >> 2;
        g_rgbImage[rgbSize].g = g_rgbImage[rgbSize].r;
        g_rgbImage[rgbSize].b = g_rgbImage[rgbSize].r;//display yellow
    }


	// Draw
	g_pRender->draw(-1, (uint8_t*)g_rgbImage, rgbSize, pFrame->width, pFrame->height, &pFrame);

    // call this while imiReadNextFrame returns 0;
    imiReleaseFrame(&pFrame);

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

    uint8_t bMirror = 0;
    uint32_t len = sizeof(uint8_t);
    imiSetDeviceProperty(pImiDevice, IMI_PROPERTY_IR_MIRROR, &bMirror, len);

	//4.imiOpenStream()
    //if ( 0 != imiOpenStream(pImiDevice, IMI_DEPTH_FRAME, NULL, NULL, &g_streams[g_streamNum++]) )
    if ( 0 != imiOpenStream(pImiDevice, IMI_IR_FRAME, NULL, NULL, &g_streams[g_streamNum++]) )
	{
        printf("Open IR Stream Failed!\n");
        goto exit;
    }
	printf("Open IR Stream Success.\n");

    //5.create window and set read Stream frame data callback
    g_pRender = new SampleRender("IR Image View", IMAGE_WIDTH, IMAGE_HEIGHT);  // window title & size
    g_pRender->init(argc, argv);
    g_pRender->setKeyCallback(keyboard);
    // read frame data callback
    g_pRender->setDataCallback(needImage, NULL);
    g_pRender->run();

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

    if (NULL != g_pRender)
	{
        delete g_pRender;
        g_pRender = NULL;
    }

	getchar();

    return 0;
}
