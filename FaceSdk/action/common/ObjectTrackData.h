#pragma once
#include <opencv2/opencv.hpp>

typedef struct ObjectSingleTraceType
{
	int type;
	float conf;
	int count;
}ObjectCameraTraceType;

typedef struct ObjectCameraTrace
{
	int channel;

	int count;
	int lost;
	time_t t_start, t_end;
	int s_start, s_end;

	cv::Rect start_pos;
	cv::Rect last_pos;

	cv::Point3i guessed_pos;

	std::vector<ObjectCameraTraceType> conf_type;
}ObjectCameraTrace;
