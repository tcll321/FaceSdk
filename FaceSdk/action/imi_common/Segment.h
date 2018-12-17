#pragma once

#include <opencv2/opencv.hpp>
#include <list>
#include <vector>
#include "MaskProcess.h"
class SegmentFactory
{
public:
	static void point_grow(cv::Mat src, cv::Mat& dst, cv::Point p, int thresh = 2, int thresh2 = 20)
	{
		using namespace std;
		using namespace cv;
		cv::Mat mask(src.size(), CV_8UC1);
		mask.setTo(0);

		list<Point> lPoint;
		lPoint.push_back(p);
		mask.at<uchar>(p) = 255;

		uchar color_base = src.at<Vec<uchar, 3> >(p)[0];

		while(!lPoint.empty())
		{
			Point p = lPoint.front();
			lPoint.pop_front();
			vector<Point> vp;

			uchar color = src.at<Vec<uchar, 3> >(p)[0];

			if (p.x >= 1)
				vp.push_back(Point(p.x - 1, p.y));
			if (p.y >= 1)
				vp.push_back(Point(p.x, p.y - 1));
			if (p.x < src.cols - 1)
				vp.push_back(Point(p.x + 1, p.y));
			if (p.y < src.rows - 1)
				vp.push_back(Point(p.x, p.y + 1));

			for (Point p2 : vp)
			{
				if(mask.at<uchar>(p2))
					continue;

				uchar color2 = src.at<Vec<uchar, 3> >(p2)[0];
				if (abs(color - color2) <= thresh && color2 >= color_base - thresh2)
				{
					mask.at<uchar>(p2) = 255;
					lPoint.push_back(p2);
				}
				else
				{
					mask.at<uchar>(p2) = 1;
				}

			}
		};

		threshold(mask, dst, 254, 255, CV_THRESH_BINARY);
	}	
	

	static void point_grow_mono(cv::Mat src, cv::Mat& dst, cv::Point p, int thresh = 2, int thresh2 = 20)
	{
		using namespace std;
		using namespace cv;
		cv::Mat mask(src.size(), CV_8UC1);
		mask.setTo(0);

		list<Point> lPoint;
		lPoint.push_back(p);
		mask.at<uchar>(p) = 255;

		uchar gray_base = src.at<uchar>(p);

		while (!lPoint.empty())
		{
			Point p = lPoint.front();
			lPoint.pop_front();
			vector<Point> vp;

			uchar color = src.at<uchar>(p);

			if (p.x >= 1)
				vp.push_back(Point(p.x - 1, p.y));
			if (p.y >= 1)
				vp.push_back(Point(p.x, p.y - 1));
			if (p.x < src.cols - 1)
				vp.push_back(Point(p.x + 1, p.y));
			if (p.y < src.rows - 1)
				vp.push_back(Point(p.x, p.y + 1));

			for (Point p2 : vp)
			{
				if (mask.at<uchar>(p2))
					continue;

				uchar color2 = src.at<uchar>(p2);
				if (abs(color - color2) <= thresh)
				{
					mask.at<uchar>(p2) = 255;
					lPoint.push_back(p2);
				}
				else
				{
					mask.at<uchar>(p2) = 1;
				}

			}
		};

		threshold(mask, dst, 254, 255, CV_THRESH_BINARY);
	}

	static cv::Point weight_center(cv::Mat src)
	{
		float x = 0, y = 0;
		int total_weight = 0;
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				uchar a = src.at<uchar>(i, j);
				if (a)
				{
					x += j*a;
					y += i*a;
					total_weight += a;
				}
			}
		}
		if (total_weight == 0)
			return cv::Point(src.cols / 2, src.rows / 2);
		else
		{
			return cv::Point(x / total_weight, y / total_weight);
		}
	}

	static cv::Point shift_center(cv::Mat src, cv::Point p, int r = 10, bool bInv = false)
	{
		float x = 0, y = 0;
		int total_weight = 0;

		uchar maxVal = 0;
		uchar minVal = 0xFF;
		int minX = (std::max)(0, p.x - r + 1);
		int maxX = (std::min)(src.cols, p.x + r);
		int minY = (std::max)(0, p.y - r + 1);
		int maxY = (std::min)(src.rows, p.y + r);
		for (int i = minY; i < maxY; i++)
		{
			for (int j = minX; j < maxX; j++)
			{
				uchar a = src.at<uchar>(i, j);
				if (a > maxVal)
					maxVal = a;
				if (a < minVal)
					minVal = a;
				if (a)
				{
					x += j*a;
					y += i*a;
					total_weight += a;
				}
			}
		}
		if (total_weight == 0)
			return p;
		else
		{
			int area = (maxX - minX)*(maxY - minY);
			if (bInv)
			{
				x = (maxX + minX - 1)*0.5*maxVal*area - x;
				y = (maxY + minY - 1)*0.5*maxVal*area - y;
				total_weight = maxVal*area - total_weight;
			}
			else
			{
				x = x - (maxX + minX - 1)*0.5*minVal*area;
				y = y - (maxY + minY - 1)*0.5*minVal*area;
				total_weight = total_weight + minVal*area;
			}
			
			return cv::Point(x / total_weight, y / total_weight);
		}
	}

	static cv::Point shift_center_32f(cv::Mat src, cv::Point p, int r = 10, bool bInv = false)
	{
		float x = 0, y = 0;
		float total_weight = 0;

		float maxVal = 0;
		float minVal = 1e6;
		int minX = (std::max)(0, p.x - r + 1);
		int maxX = (std::min)(src.cols, p.x + r);
		int minY = (std::max)(0, p.y - r + 1);
		int maxY = (std::min)(src.rows, p.y + r);
		//std::cout << "shift_center range:" << minX << "," << maxX << "," << minY << "," << maxY << std::endl;

		for (int i = minY; i < maxY; i++)
		{
			for (int j = minX; j < maxX; j++)
			{
				float a = src.at<float>(i, j);
				if (a > maxVal)
					maxVal = a;
				if (a < minVal)
					minVal = a;
			}
		}

		if (bInv)
		{
			for (int i = minY; i < maxY; i++)
			{
				for (int j = minX; j < maxX; j++)
				{
					float a = src.at<float>(i, j);
					if (a)
					{

						x += j*(2*maxVal -a);
						y += i*(2*maxVal - a);
						total_weight += (2*maxVal - a);
					}
				}
			}
		}
		else
		{
			for (int i = minY; i < maxY; i++)
			{
				for (int j = minX; j < maxX; j++)
				{
					float a = src.at<float>(i, j);
					if (a)
					{

						x += j*(a - minVal + maxVal);
						y += i*(a - minVal + maxVal);
						total_weight += (a - minVal + maxVal);
					}
				}
			}
		}

		//std::cout << "vals:" << minVal << "," << maxVal << std::endl;
		if (total_weight == 0)
			return p;
		else
		{
			return cv::Point(x / total_weight, y / total_weight);
		}
	}


	static std::vector<cv::Point> segment_shift(cv::Mat src_mask)
	{
		cv::Mat mask(src_mask.size(), CV_8UC1, cv::Scalar(0));
		int r = 20;

		std::vector<cv::Point> vp;
		for (int i = 0; i < src_mask.rows; i++)
		{
			uchar* pSrc = src_mask.data + i*src_mask.step[0];
			uchar* pMask = mask.data + i*mask.step[0];
			for (int j = 0; j < src_mask.cols; j++, pSrc++, pMask++)
			{
				if (*pSrc && !*pMask)
				{
					cv::Point p(j, i);
					cv::Point p_center = shift_center(src_mask, p, r);
					int count = 0;
					while (norm(p_center - p) >= 3 && count++ <= 5)
					{
						p = p_center;
						p_center = shift_center(src_mask, p);
					}

					vp.push_back(p_center);

					cv::circle(mask, p_center, r, cv::Scalar(255), -1);
				}
			}
		}
		return vp;
	}

	static cv::Mat hist_soft_thresh(const cv::Mat &m, cv::Mat global_hist, cv::Mat sample_hist, cv::Mat mask = cv::Mat(), float cut_ratio = 0.15)
	{
		cv::Mat cumu_hist(global_hist.size(), CV_32FC1);
		cumu_hist.at<float>(0) = global_hist.at<float>(0);
		for (int i = 1; i < global_hist.rows; i++)
		{
			cumu_hist.at<float>(i) = cumu_hist.at<float>(i - 1) + global_hist.at<float>(i);
		}
		for (int i = 0; i < cumu_hist.rows; i++)
		{
			cumu_hist.at<float>(i) /= cumu_hist.at<float>(cumu_hist.rows - 1);
		}
#if 0
		double GlobalHist[256] = { 0 };
		for (int i = 0; i < 256; i++)
			GlobalHist[i] = global_hist.at<float>(i);
		double SampleHist[256] = { 0 };
		for (int i = 0; i < 256; i++)
			SampleHist[i] = sample_hist.at<float>(i);

		float ratio_global_min = 0, ratio_global_max = 0;
		float min_global = MaskProcess::otsu_thresh(GlobalHist, ratio_global_min, 0.1, 0.3);
		float max_global = MaskProcess::otsu_thresh(GlobalHist, ratio_global_max, 0.7, 0.9);
		float ratio_global_mean = 0;
		float mean_global = MaskProcess::otsu_thresh(GlobalHist, ratio_global_mean, min_global, max_global);

		float ratio_sample = 0;
		float max_sample = MaskProcess::otsu_thresh(SampleHist, ratio_sample, 0.25, 0.7);
#endif	
		//std::cout << cumu_hist;
		cv::Mat prob(m.size(), CV_32FC1);
		prob.setTo(0);
		for (int i = 0; i < prob.rows; i++)
		{
			for (int j = 0; j < prob.cols; j++)
			{
				if(!mask.empty() && !mask.at<uchar>(i,j))
					continue;

				int bin = m.at<uchar>(i, j)*global_hist.rows / 256;
				//std::cout << (int)m.at<uchar>(i, j) << "," <<bin << std::endl;
				float p = cumu_hist.at<float>(bin);
				//if (p > 0)
				//	std::cout << p << std::endl;
				if (p < cut_ratio)
				{
					prob.at<float>(i, j) = 1 - p;
				}
			}
		}
		return prob;
	}
};