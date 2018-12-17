#pragma once
#include "BasicConfig.h"
#include <opencv2/opencv.hpp>
#include "ColorCameraConfig.h"

class YoloConfig :public BasicConfig
{
public:
	bool load(const char* file, const char* title = "YOLO")
	{
		szTitle = title;
		ReadIni::load_string(title, "config", 0, file, config);
		ReadIni::load_string(title, "weights", 0, file, weights);
		ReadIni::load_string(title, "labels", 0, file, labels);
		ReadIni::load_param<float>(title, "class_thresh", class_thresh, file);
		ReadIni::load_param<float>(title, "nms_thresh", nms_thresh, file);
		return true;
	}

	std::string config = "net.cfg";
	std::string weights = "net.weights";
	std::string labels = "net.names";

	float class_thresh = 0.5;
	float nms_thresh = 0.4;
};

static std::ostream& operator<<(std::ostream& s, const YoloConfig& config)
{
	s << "[" << config.szTitle << "]" << std::endl;
	s << "config = " << config.config << std::endl;
	s << "weights = " << config.weights << std::endl;
	s << "labels = " << config.labels << std::endl;
	s << "class_thresh = " << config.class_thresh << std::endl;
	s << "nms_thresh = " << config.nms_thresh << std::endl;
	return s;
}
