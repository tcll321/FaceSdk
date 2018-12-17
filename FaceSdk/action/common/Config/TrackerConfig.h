#pragma once

#include "BasicConfig.h"

class SingleTrackerConfig :public BasicConfig
{
public:
	bool load(const char* file, const char* title = "SINGLE_TRACKER")
	{
		szTitle = title;
		ReadIni::load_param<int>(title, "lost_count", lost_count, file);		
		ReadIni::load_param<float>(title, "min_conf", min_conf, file);
		ReadIni::load_param<float>(title, "max_dist", max_dist, file);

		ReadIni::load_param<float>(title, "IOU_same", IOU_same, file);
		ReadIni::load_param<float>(title, "IOU_diff", IOU_diff, file);
		ReadIni::load_param<float>(title, "area_diff", area_diff, file);

		ReadIni::load_param<float>(title, "nms_ratio", nms_ratio, file);
		return true;
	}

	//int back_trace_count = 5;
	int lost_count = 5;

	float min_conf = 0.5;
	float max_dist = 1.5;

	float IOU_same = 0.25;
	float IOU_diff = 0.65;

	float nms_ratio = 0.8;
	float area_diff = 0.5;
};

inline std::ostream& operator<<(std::ostream& s, const SingleTrackerConfig& cfg)
{
	s << "[" << cfg.szTitle << "]" << std::endl;
	s << "lost_count = " << cfg.lost_count << std::endl;
	s << "min_conf = " << cfg.min_conf << std::endl;
	s << "IOU_same = " << cfg.IOU_same << std::endl;
	s << "IOU_diff = " << cfg.IOU_diff << std::endl;
	s << "nms_ratio = " << cfg.nms_ratio << std::endl;
	s << "area_diff = " << cfg.area_diff << std::endl;
	s << "max_dist =" << cfg.max_dist << std::endl;
	return s;
}