#pragma once

#include <opencv2/opencv.hpp>
#include "MaskProcess.h"

class Shineness
{
public:
	static int light_mask(cv::Mat src, cv::Mat & shine, float thresh = 20, cv::Mat mask = cv::Mat(), float ratio = 0.8, bool bMorph = true)
	{
		cv::Mat gray;
		if (src.channels() == 3)
			cv::cvtColor(src, gray, CV_BGR2GRAY);
		else
			gray = src;

		if (!mask.empty())
			gray.setTo(0, ~mask);
		double cMin, cMax;
		cv::minMaxLoc(gray, &cMin, &cMax);
		
		cv::Scalar sMean = cv::mean(gray, mask);
		double cMean = sMean[0];

		if (cMax < cMean + thresh)
		{
			shine.create(src.size(), CV_8UC1);
			shine.setTo(0);
			return 1;
		}
			
		int cThresh = cMax*ratio + cMean*(1-ratio);
		cv::threshold(gray, shine, cThresh, 255, CV_THRESH_BINARY);
		if (!mask.empty())
			shine &= mask;

		if (bMorph)
			cv::morphologyEx(shine, shine, CV_MOP_CLOSE, cv::Mat(5, 5, CV_8UC1, 1), cv::Point(-1, -1), 1);
	
		return 0;
	};

	static int dark_mask(cv::Mat src, cv::Mat& dark, cv::Mat mask = cv::Mat(), float ratio = 0.0)
	{
		float thresh = 0, thresh2 = 0;
		float dark_cut = 0;
		float light_cut = 0.85;
		cv::Mat center_weight = MaskProcess::generate_gaussian(src.size(), cv::Point(src.cols / 2, src.rows/2), 12000, 8000);
		
		//cv::imshow("dark_mask_weight", ShowUtils::show_32F(center_weight));
		
		cv::Mat t = MaskProcess::masked_otsu(src, mask, center_weight, thresh, true, 0.0, ratio, dark_cut, light_cut);
		//cv::Mat t2;// = MaskProcess::masked_otsu(src, cv::Mat(), cv::Mat(), thresh2, true);
		//cv::threshold(src, t2, 0, 255, CV_THRESH_OTSU);
		//cv::imshow("dark_otsu2", t2);
		//dark = 255 - t;
		dark = t;
		if (!mask.empty())
			dark &= mask;

		return 0;
	}
};