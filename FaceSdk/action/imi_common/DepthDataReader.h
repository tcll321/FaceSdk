#pragma once

#include <opencv2/opencv.hpp>

class DepthDataReader
{
public:
	static cv::Mat read(std::string file, int w = 320, int h = 240)
	{
		std::ifstream ifs(file, std::ios_base::binary);
		if (!ifs.is_open())
			return cv::Mat();
		cv::Mat image(h, w, CV_16UC1);
		ifs.read((char*)image.data, image.step[0] * image.rows);
		return image;
	}
	static void write(std::string file, cv::Mat m)
	{
		FILE* file_depth = fopen(file.c_str(), "wb");
		for(int i = 0;i < m.rows;i++)
		{
			fwrite(m.data + i*m.step[0], sizeof(float), m.cols, file_depth);
		}
		fclose(file_depth);		
	}
};