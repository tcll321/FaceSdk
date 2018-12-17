#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include "MaskProcess.h"
#include "Shineness.h"
#include "Hist.h"

class DetectPupil
{
public:
	static cv::Mat down_mask(cv::Mat src, std::vector<cv::Point> vUp, std::vector<cv::Point> vDown, std::vector<int> label)
	{
		std::vector<cv::Point> vp, vd;
		for (int i = 0; i < vUp.size(); i++)
		{
			if (label[i])
			{
				vp.push_back(vUp[i]);
				vd.push_back(vDown[i]);
			}
		}

		float max_gap = 0;
		int max_gap_id = -1;
		for (int i = 0; i < vp.size(); i++)
		{
			float gap = vd[i].y - vp[i].y;
			if (gap >= max_gap)
			{
				max_gap = gap;
				max_gap_id = i;
			}
		}

		cv::Mat mask(src.size(), CV_8UC1);
		mask.setTo(0);
		if (max_gap <= 0)
			return mask;
		std::vector<cv::Point> vdd;
		for (int i = vd.size() - 1; i >= 0; i--)
		{
			if (vd[i].y - vp[i].y >= max_gap*0.7)
				vdd.push_back(vd[i]);
		}
	
		vp.insert(vp.end(), vdd.begin(), vdd.end());
		
		cv::fillPoly(mask, std::vector<std::vector<cv::Point> >(1, vp), cv::Scalar(255, 255, 255));
		cv::dilate(mask, mask, cv::Mat());
		return mask;
	}
	static int light_spot(cv::Mat src, std::vector<cv::Point> & vLight, cv::Mat mask) //here mask is the eye mask
	{
		if (src.channels() == 3)
		{
			cv::Mat gray;
			cv::cvtColor(src, gray, CV_BGR2GRAY);
			src = gray;
		}
		//use otsu to split dark and light area

		float thresh = 0;
		cv::Mat maskLight = MaskProcess::masked_otsu(src, mask, cv::Mat(), thresh, false, 0.75);
		cv::Mat maskDark = (~maskLight) & mask;

		cv::Mat eq;
		HistMeter::equalize_hist_mask(src, eq, mask);
		//cv::imshow("D_eq", eq);

		//cv::imshow("D0", maskDark);
		cv::Mat shine;

		cv::Mat se(15, 15, CV_8U, cv::Scalar(1));
		cv::morphologyEx(maskDark, maskDark, cv::MORPH_CLOSE, se, cv::Point(-1, -1), 1);
		//cv::imshow("D1", maskDark);
		Shineness::light_mask(src, shine, 0, maskDark, 0.5, true);
		//cv::imshow("maskLL", shine);
	
		auto vs = SegmentFactory::segment_shift(shine);

		cv::Point dark_center = SegmentFactory::weight_center(maskDark);

		float min_dist = 1e6;
		int min_id = -1;
		for (int i = 0; i < vs.size(); i++)
		{
			float dist = norm(dark_center - vs[i]);
			if (dist <= min_dist)
			{
				min_dist = dist;
				min_id = i;
			}
		}

		if (min_id >= 0)
			vLight.push_back(vs[min_id]);

	//	cv::Mat dark;
	//	Shineness::light_mask(255 - src, dark, 0, maskDark, 0.7, false);
	//	cv::imshow("DD", dark);

	//	cv::imshow("LD", dark&shine);
#if 0
		auto mean_light = cv::mean(src, maskLight);
		cv::Mat maskLL;
		cv::threshold(src, maskLL, mean_light[0]*1.1, 255, CV_THRESH_BINARY);
		maskLL &= mask;
		cv::imshow("maskLL", maskLL);
#endif
		return 0;
	}

	static int hough_circle(cv::Mat src, std::vector<cv::Vec3f > &v)
	{
		cv::Mat gray;
		if (src.channels() == 3)
			cv::cvtColor(src, gray, CV_BGR2GRAY);
		else
			gray = src;
		cv::Mat eq = gray;
		//equalizeHist(gray, eq);
		//cv::imshow("eq", eq);

		cv::HoughCircles(eq, v, CV_HOUGH_GRADIENT, 2, 10, 5);

		cv::Mat hou = src.clone();
		for (int i = 0; i < v.size(); i++)
		{
			cv::circle(hou, cv::Point(v[i][0], v[i][1]), v[i][2], cv::Scalar(0, 0, 255), 2);
		}
		//cv::imshow("hou", hou);
		return 0;
	}

	static void center_grad(cv::Mat dx, cv::Mat dy, cv::Mat& res, cv::Point center, cv::Mat mask = cv::Mat(), float maxSinY = -1)
	{
		res.create(dx.size(), CV_32FC1);
		res.setTo(0);
		for (int i = 0; i < dx.rows; i++)
		{
			float* pDx = (float*)(dx.data + i*dx.step[0]);
			float* pDy = (float*)(dy.data + i*dy.step[0]);
			float* pRes = (float*)(res.data + i*res.step[0]);
			uchar* pMask = mask.empty() ? 0 : (mask.data + i*mask.step[0]);
			for (int j = 0; j < dx.cols; j++, pDx++, pDy++, pRes++, pMask++)
			{
				if (pMask && *pMask == 0)
					continue;
#if 1
				float n = (*pDx)*(*pDx) + (*pDy)*(*pDy);
				if(maxSinY > 0 && fabs(*pDy/(n+0.001)) > maxSinY)
					continue;

				if ((-*pDx)*(center.x - j) + (-*pDy)*(center.y - i) >= sqrtf(n *((center.x - j)*(center.x - j) + (center.y - i)*(center.y - i))) / 2)
				{
					*pRes = n;
				}
#else
				cv::Point2f grad(-*pDx, -*pDy);
				float n = cv::norm(grad);

				if (maxSinY > 0 && fabs(grad.y/(n + 0.01))>maxSinY)
					continue;

				cv::Point2f arrow(center.x - j, center.y - i);
				arrow *= (1.0/cv::norm(arrow));
				if (grad.dot(arrow) >= n*0.5)
				{
					*pRes = n;
				}
#endif
			}
		}
	}

	static int ranged_max_exp(cv::Mat arr, int x_min, int x_max, bool bRight)
	{
		if (x_min >= x_max)
			return x_min;
		float max_val = 0;
		float max_id = -1;

		for (int i = x_min; i < x_max; i++)
		{
			float val = arr.at<float>(i);
			
			if (val > max_val)
			{
				max_val = val;
				max_id = i;
			}
		}

		int x_min_2 = bRight ? x_min*0.1 + max_id*0.9 : x_min;
		int x_max_2 = bRight ? x_max : x_max*0.1 + max_id*0.9;

		float mean_val_x = 0;
		float mean_count = 0;
	
		float mean_upper_val_x = 0;
		float mean_upper_count = 0;
		for (int i = x_min_2; i <= x_max_2; i++)
		{
			float val = arr.at<float>(i);
			mean_val_x += val*i;
			mean_count += val;
			if (val > max_val/2)
			{
				mean_upper_val_x += val*i;
				mean_upper_count += val;
			}
		}
		mean_val_x /= (0.0001 + mean_count);
		mean_upper_val_x /= (mean_upper_count + 0.0001);

		//std::cout << "means:" << mean_upper_val_x << "," << mean_val_x << "," << max_id << std::endl;
		
		return 0.2*mean_upper_val_x + 0.1*mean_val_x + 0.7*max_id;
			//max_id;
	};

	static std::vector<int> iris_x(cv::Mat iris_hist, cv::Point light_point, float range = 25, int cut = 0)
	{
		cv::Mat iris_hist_w = iris_hist.clone();
		for (int i = 0; i < iris_hist_w.rows; i++)
		{
			float scale = 1 - 0.8 * exp(-pow(1.0*(light_point.x - i), 2) / (range*range/2));
			iris_hist_w.at<float>(i) *= scale;
		}
		if (cut > 0)
		{
			for (int i = 0; i < light_point.x - cut; i++)
			{
				float scale = exp(-pow(1.0*(light_point.x - cut - i), 2) / (range*range));
				iris_hist_w.at<float>(i) *= scale;
			}
			for (int i = light_point.x + cut; i < iris_hist_w.rows - 1; i++)
			{
				float scale = exp(-pow(1.0*(light_point.x + cut - i), 2) / (range*range));
				iris_hist_w.at<float>(i) *= scale;
			}
		}


		//cv::imshow("scaledH", HistMeter::draw(iris_hist_w, 1));
		std::vector<int> v;

		v.push_back(ranged_max_exp(iris_hist_w, 0, light_point.x, 0));
		v.push_back(ranged_max_exp(iris_hist_w, light_point.x, iris_hist_w.rows - 1, 1));

		return v;
	}
};