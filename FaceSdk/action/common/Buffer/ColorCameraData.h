#pragma once

#include <opencv2/opencv.hpp>
#include <chrono>
#include "../BasicStructs.h"

class ColorCameraData
{
public:
	cv::Mat data;
	int channel;
};


class ColorCameraBatch
{
public:
	std::vector<shop::Frame> vData;
};