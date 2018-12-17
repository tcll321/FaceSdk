#pragma once

#include <opencv2/opencv.hpp>

class Hough
{
public:
	static std::vector<cv::Vec3f> search(cv::Mat src, cv::Mat mask,
		std::vector<float> r_range, std::vector<float> x_range, std::vector<float> y_range,
		std::vector<float> direction_weight)
	{
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{

			}
		}
	}
};