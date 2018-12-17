#pragma once

#include <opencv2/opencv.hpp>
#include <atomic>
#include <thread>
#define USE_A100 1
#include "ImageSource.h"
#include "../common/Config/ShelfConfig.h"
#include "../common/Buffer/QueueBuffer.h"
#include<memory>
class SyncShelfCameraManager
{
public:
	SyncShelfCameraManager();
	~SyncShelfCameraManager();
	SyncShelfCameraManager(const ShelfConfig& shelf, const ColorCameraGroupConfig& colorGroup, const DepthCameraGroupConfig& depthGroup);

	void init(const ShelfConfig& shelf, const ColorCameraGroupConfig& colorGroup, const DepthCameraGroupConfig& depthGroup);
	void start();
	void stop();

	bool fetch(std::vector<cv::Mat> & vColor, std::vector<cv::Mat> & vDepth, std::vector<cv::Mat>* vSkeleton = 0);
	bool fetch_back(std::vector<cv::Mat> & vColor, std::vector<cv::Mat> & vDepth, std::vector<cv::Mat>* vSkeleton = 0);

	void _proc_image(ImageSource* p, QueueBuffer<cv::Mat>* image ,int flag, QueueBuffer<cv::Mat>* pSkeleton);
	
	bool _add_color_camera(ColorCameraConfig config);
	bool _add_depth_camera(DepthCameraConfig config);

	std::atomic<cv::Mat*> m_colorBuf[16], m_depthBuf[16];
	std::atomic<int> m_countDown;
	std::thread m_color_thread[16];
	std::thread m_depth_thread[16];
	int m_colorThreadCount, m_depthThreadCount;
	bool bStart, bStop;
	std::vector<std::shared_ptr<ImageSource> > vSource;

	std::vector<ColorCameraConfig> m_colorConfig;
	std::vector<DepthCameraConfig> m_depthConfig;

	std::vector<QueueBuffer<cv::Mat>* > m_colorQueue;
	std::vector<QueueBuffer<cv::Mat>* > m_depthQueue;
	std::vector<QueueBuffer<cv::Mat>* > m_skeletonQueue;
};