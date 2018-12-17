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

#include "../Common/Render.h"
#include "ImiCamera.h"

//#define IMAGE_WIDTH			640
//#define IMAGE_HEIGHT			480
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


// window handle
SampleRender*	g_pRender = NULL;

ImiCameraHandle g_cameraDevice = NULL; 

int IMAGE_WIDTH   = 640;
int IMAGE_HEIGHT = 480;
bool bStop = false;

bool bSave = false;

uint8_t g_bmpColor[1920*1080*3] = {0};



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

int32_t takePhoto(const char* bmpImagePath, const ImiCameraFrame* pframe)
{
    BMPFILEHEADER_T	bmfh;			// bitmap file header
    BMPINFOHEADER_T	bmih;			// bitmap info header (windows)

    const int OffBits = 54;

    int32_t imagePixSize = pframe->width * pframe->height;

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
    bmih.biWidth = pframe->width;
    bmih.biHeight = -pframe->height;


    memcpy( (void*)g_bmpColor, pframe->pData, imagePixSize*3);

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
    static RGB888Pixel	g_rgbImage[1920 * 1080] = {0};

    if(bStop) {
        return false;
    }


    ImiCameraFrame* pCamFrame = NULL;
    if(0 != imiCamReadNextFrame(g_cameraDevice, &pCamFrame, 40)) {
        return false;
    }

    if (pCamFrame == NULL) {
        return false;
    }

    memcpy((void*)&g_rgbImage, (const void*)pCamFrame->pData, pCamFrame->size);

    if(bSave) {
        FILE* pFile = fopen("Color_reg.raw", "wb");

        fwrite(pCamFrame->pData, pCamFrame->size, 1, pFile);

        fclose(pFile);

        takePhoto("Color_reg.bmp", pCamFrame);

        bSave = false;
    }

    g_pRender->initViewPort();
    WindowHint hint(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    g_pRender->draw((uint8_t*)g_rgbImage, pCamFrame->size, hint);

    // call this while imiReadNextFrame returns 0;
    imiCamReleaseFrame(&pCamFrame);

    g_pRender->update();

    return true;
}

int main(int argc, char** argv)
{
    if(0 != imiCamOpen(&g_cameraDevice)) {
        printf("Open UVC Camera Failed!\n");
        getchar();
        return -1;
    }

    ImiCameraFrameMode frameMode = {CAMERA_PIXEL_FORMAT_RGB888, IMAGE_WIDTH, IMAGE_HEIGHT, 30};
    if(0 != imiCamStartStream(g_cameraDevice, &frameMode)) {
        printf("Start Camera stream Failed!\n");
        imiCamClose(g_cameraDevice);

        getchar();
        return -1;
    }
	printf("Open Color Stream Success.\n");

    //6.create window and set read Stream frame data callback
    g_pRender = new SampleRender("RGB Color View", IMAGE_WIDTH, IMAGE_HEIGHT);  // window title & size
    g_pRender->init(argc, argv);
    g_pRender->setKeyCallback(keyboard);
    g_pRender->setDataCallback(needImage, NULL);
    g_pRender->run();

exit:
	//7.imiCloseStream()
    if(NULL != g_cameraDevice) {
        imiCamStopStream(g_cameraDevice);
        imiCamClose(g_cameraDevice);
        g_cameraDevice = NULL;
    }

    if (NULL != g_pRender)
	{
        delete g_pRender;
        g_pRender = NULL;
    }

	getchar();

    return 0;

}
