/********************************************************************************
  Copyright (c) 2015 Huajie IMI Technology Co., Ltd.
  All rights reserved.

  @File Name     : readme.txt
  @Author        : Zhang hongliang
  @Date          : 2015-06-19
  
  @Description   : 本文档介绍各example的内容、用法以及相关注意事项
  
  @Version       : 0.1.0
  @History       :
  1.2015-06-19 Zhang Hongliang Created file
********************************************************************************/
  
ImiSDK
    |
    |_3rdparty  第三方库
    |    |_d3d
    |    |_freeglut-2.8.1
    |    |_lib 
    |
    |_Documentation
    |
    |_Driver   ImiDevice驱动安装程序，自动安装。注意Win8,Win10系统下需禁用驱动程序安装强制签名才能安装成功。        
    |
    |_Examples SDK使用例子，使用VS2010编写.
    |
    |_Include  ImiSDK头文件
    |
    |_Redist   Imi动态库ImiNect.dll、ImiNect.lib、ImiSensor.dll
    |
    |_Tools    演示程序
     
           

ColorDepthViewer
	ColorDepthViewer演示[主动获取彩色图和深度图帧数据]的流程.
	该例创建了2个channel实例,类型分别是IMI_COLOR_FRAME[彩色图],IMI_DEPTH_FRAME[深度图];
	通过setDataCallback(needImage, NULL), 窗口循环调用needImage(),执行imiWaitForStream以及imiReadNextFrame，
	主动获取帧数据,并显示到窗口中.
	
ColorViewer	
	colorViewer演示[主动获取彩色图帧数据]的流程.
	该例创建了1个channel实例，类型为IMI_COLOR_FRAME[彩色图].
	通过setDataCallback(needImage, NULL), 窗口循环调用needImage(),执行imiWaitForStream以及imiReadNextFrame，
	主动获取帧数据,并显示到窗口中.
	
DepthViewer
	DepthViewer演示[主动获取深度图帧数据]的流程
	该例创建了1个channel实例,类型是IMI_DEPTH_FRAME[深度图];
	通过setDataCallback(needImage, NULL), 窗口循环调用needImage(),执行imiWaitForStream以及imiReadNextFrame，
	主动获取帧数据,并显示到窗口中.
	
PointCloudViewer
	PointCloudViewer演示[主动获取深度图帧数据并进行点云转换显示点云图]的流程
	该例创建了1个channel实例,类型是IMI_DEPTH_FRAME[深度图];
	通过setDataCallback(needImage, NULL), 窗口循环调用needImage(),执行imiWaitForStream、imiReadNextFrame以及imiConvertDepthToPointCloud，
	主动获取帧数据,并转换为点云数据，并显示到窗口中.

UserTracker
	UserTracker演示[主动获取前景和骨架数据]的流程.
	该例创建了1个channel实例,类型为IMI_USER_INDEX_SKELETON_FRAME[骨架图];
	通过imiRegisterChannelCallback注册回调,窗口循环调用needImage()，执行imiWaitForStream以及imiReadNextFrame，
	主动获取帧数据,并显示到窗口中.

UserSegmentation
	UserSegmentation演示背景图片中插入人物的图片.
	该例获取IMI_COLOR_FRAME[彩色图]和IMI_USER_INDEX_SKELETON_FRAME[骨架图];
	根据骨架图中前景图人物的位置，抠下彩色图的对应位置，贴到图片中显示出来。

Skeleton
	Skeleton演示人的骨架.
	该例获取IMI_USER_INDEX_SKELETON_FRAME[骨架图]，将骨架图中每个点位连接起来。
	
	
工程使用骨架流时需要将 Redist\Win32 目录下 NiTE2目录、OpenNI2目录、NiTE.ini、OpenNI.ini 拷贝至运行目录

	
Render.h,.cpp、Render2.h,.cpp中是openGL实现的展示界面,需注意:	

1）Windows下，需要freeglut库;
	库头文件与freeglut.lib文件位于【ImiSDK安装目录3rdparty\freeglut-2.8.1】。
	freeglut.dll位于【ImiSDK安装目录3rdparty3rdparty/lib】
	
	
2）Ubuntu:
sudo apt-get install freeglut2.8.1 freeglut2.8.1-dev 
获取并安装freeglut

3) Linux:
需要编译Freeglut源码
cd 3rdparty/freeglut-2.8.1
./autogen.sh
./configure
make