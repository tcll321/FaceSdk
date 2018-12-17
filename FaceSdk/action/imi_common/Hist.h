#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
//#include "ShowUtils.hpp"

class HistMeter
{
public:
	static cv::Mat calucate(cv::Mat m, cv::Mat mask = cv::Mat(), int bins = 64)
	{
		if (m.channels() == 3)
		{
			cv::Mat gray;
			cv::cvtColor(m, gray, CV_BGR2GRAY);
			m = gray;
		}
		cv::Mat hist;
		cv::calcHist(std::vector<cv::Mat>(1, m), std::vector<int>(1, 0), mask, hist, { bins }, { 0,256 });
		//std::cout << "hist (" << hist.rows << "," << hist.cols << ")\n";
		return hist;
	}
	static cv::Mat draw(cv::Mat hist, int bin_width = 10)
	{
		double vMax = 1000;
		//double vMin, vMax;
		//cv::minMaxLoc(hist, &vMin, &vMax);
	//	int bin_width = 10;
		cv::Mat graph(200, hist.rows*bin_width, CV_8UC3);
		graph.setTo(0);

		for (int i = 0; i < hist.rows; i++)
		{
			float v = hist.at<float>(i);
			int h = v*graph.rows / vMax;
			cv::Rect r = cv::Rect(i*bin_width, graph.rows - h, bin_width, h);
			float ratio = 1.0*i / hist.rows;
			auto color = ShowUtils::level_color(ratio);
			cv::rectangle(graph, r, color, -1);
		}
		return graph;
	}
	static void equalize_hist_mask(cv::Mat src, cv::Mat& dst, cv::Mat mask = cv::Mat())
	{
		if (src.channels() == 3)
		{
			cv::Mat gray;
			cv::cvtColor(src, gray, CV_BGR2GRAY);
			src = gray;
		}
		if (mask.empty())
		{
			cv::equalizeHist(src, dst);
			return;
		}
		else
		{
			double Histogram[256] = { 0 }; //灰度直方图，下标是灰度值，保存内容是灰度值对应的像素点总数  									   //	uchar *data = src.data;
			double totalNum = 0; // image.rows*image.cols; //像素总数  
			for (int i = 0; i < src.rows; i++)   //为表述清晰，并没有把rows和cols单独提出来  
			{
				uchar* pSrc = src.data + i*src.step[0];
				uchar* pMask = mask.data + i*mask.step[0];
				for (int j = 0; j < src.cols; j++, pSrc++, pMask++)
				{
					if (*pMask)
					{
						Histogram[*pSrc]++;
						totalNum++;
					}
				}
			}

			if (totalNum == 0)
			{
				dst = src;
				return;
			}
			double AddHist[256] = { 0 };
			AddHist[0] = Histogram[0];
			for (int i = 1; i < 256; i++)
				AddHist[i] = AddHist[i - 1] + Histogram[i];
			for (int i = 0; i < 256; i++)
			{
				AddHist[i] = AddHist[i] * 255 / AddHist[255];
				//				std::cout << "A[" << i << "]:" << AddHist[i] << std::endl;
			}

			dst = src.clone();

			for (int i = 0; i < src.rows; i++)
			{
				uchar* pSrc = src.data + i*src.step[0];
				uchar* pMask = mask.data + i*mask.step[0];
				uchar* pDst = dst.data + i*dst.step[0];
				for (int j = 0; j < src.cols; j++, pSrc++, pMask++, pDst++)
				{
					if (*pMask)
					{
						*pDst = AddHist[*pSrc];
					}
				}
			}
		}
	}

	static void calculate_horizonal_hist_32F(cv::Mat src, cv::Mat & dst, float trunck = -1)
	{
		dst.create(src.cols, 1, CV_32FC1);
		dst.setTo(0);
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				dst.at<float>(j) += trunck > 0? ((std::min)(trunck, src.at<float>(i,j))):src.at<float>(i, j);
			}
		}
	}
};