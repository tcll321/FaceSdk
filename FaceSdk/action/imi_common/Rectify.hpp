#pragma once
#include <opencv2/opencv.hpp>
#include "RandGeneratorBasic.h"

namespace ImageProcess
{
	cv::Point2f pivot(cv::Point2f p1, cv::Point2f p2)
	{
		cv::Point2f center = (p1 + p2) *0.5;
		cv::Point2f dir = p2 - center;
		return cv::Point2f(center.x - dir.y, center.y + dir.x);
	}
	int RectifyCut(const cv::Mat src, cv::Mat& dst, cv::Rect r, std::vector<cv::Point2f> land, int dstSize = 224, int minSize = 150)
	{
		if (r.width < minSize || r.height < minSize || land.empty())
			return -1;

		//float angle = atan2f(land[0].y - land[1].y, land[0].x - land[1].x);
		std::vector<cv::Point2f> vSrc, vDst;
		vSrc.push_back(land[0]);
		vDst.push_back(cv::Point2f(dstSize * 0.3, dstSize * 0.38));
		vSrc.push_back(land[1]);
		vDst.push_back(cv::Point2f(dstSize * 0.7, dstSize * 0.38));
		vSrc.push_back(pivot(land[0], land[1]));
		vDst.push_back(pivot(vDst[0], vDst[1]));
		cv::Mat rot = cv::getAffineTransform(vSrc, vDst);
		cv::warpAffine(src, dst, rot, cv::Size(dstSize, dstSize));
#if 0
		for (auto p:vDst)
		{
			cv::circle(dst, p, 1, cv::Scalar(0, 255, 0), -1);
		}
#endif
		return 0;
	}
	int RectifyRelative(const cv::Mat src, cv::Mat& dst, cv::Rect r, std::vector<cv::Point2f> land, float ratio = 1.4, int dstSize = 256, int minSize = 50)
	{
		if (r.width < minSize || r.height < minSize || land.empty())
			return -1;

		cv::Point2f center = (land[0] + land[1]) *0.5;
		cv::Point2f pv = pivot(land[0], land[1]);

		float d = cv::norm(center - land[0]);

		cv::Point2f center_ratio(1.0*(center.x - r.x)/ r.width, 1.0*(center.y - r.y)/ r.height);
		float inner_size_dst = dstSize / ratio;
		cv::Point2f center_dst(0.5*(dstSize - inner_size_dst) + inner_size_dst*center_ratio.x, 0.5*(dstSize - inner_size_dst) + inner_size_dst*center_ratio.y);
		float d_dst = d*inner_size_dst / r.width;
		cv::Point2f p0_dst = center_dst - cv::Point2f(d_dst, 0);
		cv::Point2f p1_dst = center_dst + cv::Point2f(d_dst, 0);
		cv::Point2f pv_dst = pivot(p0_dst, p1_dst);
		std::vector<cv::Point2f> vSrc = { land[0], land[1], pv };
		std::vector<cv::Point2f> vDst = { p0_dst,p1_dst,pv_dst };
		cv::Mat rot = cv::getAffineTransform(vSrc, vDst);

		cv::warpAffine(src, dst, rot, cv::Size(dstSize, dstSize));
		//cv::circle(dst, center_dst, 2, cv::Scalar(0, 255, 0), -1);
		//cv::circle(dst, p0_dst, 2, cv::Scalar(0, 255, 0), -1);
		//cv::circle(dst, p1_dst, 2, cv::Scalar(0, 255, 0), -1);
		//cv::circle(dst, pv_dst, 2, cv::Scalar(0, 255, 0), -1);
		return 0;
	}

	int Augment(const cv::Mat src, std::vector<cv::Mat>& dst, int dstSize = 224, float angle = 1.5f, float dx = 3.0f, float dy = 3.0f, float sigma = 1.5)
	{
		if (dstSize > src.cols || dstSize > src.rows)
			return -1;
		int max_offset_x = (src.cols - dstSize) / 2 * 0.8;
		int max_offset_y = (src.rows - dstSize) / 2 * 0.8;

		cv::Point2f fixed_offset[9] = { cv::Point2f(0,0),
			cv::Point2f(-dx, 0), cv::Point2f(dx, 0),
			cv::Point2f(0, -dy), cv::Point2f(0, dy),
			cv::Point2f(-dx, dy), cv::Point2f(dx, dy),
			cv::Point2f(dx, -dy), cv::Point2f(dx, -dy) };
		for (int i = 0; i < 9; i++)
		{
			cv::Mat rot = cv::getRotationMatrix2D(cv::Point2f(src.cols / 2, src.rows / 2), RandB::NF(0, angle*0.8, -angle, angle), 1.0);
			cv::Mat sRot;
			cv::warpAffine(src, sRot, rot, src.size());
			cv::Point2f offset = fixed_offset[i] + cv::Point2f(RandB::NF(0, sigma, -20, 20), RandB::NF(0, sigma, -20, 20));
			offset.x = (std::min)(1.0f*max_offset_x, (std::max)(offset.x, -1.0f*max_offset_x));
			offset.y = (std::min)(1.0f*max_offset_y, (std::max)(offset.y, -1.0f*max_offset_y));
			cv::Rect rect(src.cols *0.5 + offset.x - dstSize*0.5, src.rows *0.5 + offset.y - dstSize*0.5, dstSize, dstSize);
			cv::Mat dd = sRot(rect);
			bool bFlip = RandB::BI(0.5);
			if (bFlip)
				cv::flip(dd, dd, 1);
			dst.push_back(dd);
		}
		return 0;
	}

	float Rotate(cv::Mat src, cv::Mat& dst, float angle = 5.0f, bool bUpChannel = true)
	{
		
		if (bUpChannel && src.channels() == 3)
		{
			cv::Mat src2;
			cv::cvtColor(src, src2, CV_BGR2BGRA);
			src = src2;
			dst.create(src.size(), CV_8UC4);
			dst.setTo(cv::Scalar(0, 0, 0, 0));
		}
		float a = RandB::NF(0, angle*0.8, -angle, angle);
		cv::Mat rot = cv::getRotationMatrix2D(cv::Point2f(src.cols / 2, src.rows / 2), 
			a, 1.0);
		cv::warpAffine(src, dst, rot, src.size(), 1, cv::BORDER_TRANSPARENT);
		if (RandB::BI(0.5))
			cv::flip(dst, dst, 1);
		return a;
	}
};