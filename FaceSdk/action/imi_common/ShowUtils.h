#pragma once

#include <opencv2/opencv.hpp>
#ifdef WIN32
#include <windows.h>
#include <time.h>
#endif

#define LEVEL_COLOR_MAX 1280
#define level_color_u(deltaC, pix) \
	{\
		deltaC = MAX(0, MIN(1279, (1279 - deltaC)));\
       	if (deltaC <= 255)\
		{\
			*(pix++) = 255 - deltaC;\
			*(pix++) = 0;\
			*(pix++) = 255;\
		}\
		else if (deltaC <= 511)\
		{\
			*(pix++) = 0;\
			*(pix++) = deltaC - 256;\
			*(pix++) = 255;\
		}\
		else if (deltaC <= 767)\
		{\
			*(pix++) = 0;\
			*(pix++) = 255;\
			*(pix++) = 255 - (deltaC - 512);\
		}\
		else if (deltaC <= 1023)\
		{\
			*(pix++) = deltaC - 768;\
			*(pix++) = 255;\
			*(pix++) = 0;\
		}\
		else\
		{\
			*(pix++) = 255;\
			*(pix++) = 255 - (deltaC - 1024);\
			*(pix++) = 0;\
		}\
	}

class ShowUtils
{
public:
	static cv::Mat auto_resize(cv::Mat m)
	{
		float scale = (std::min)(1000.0 / m.cols, 700.0 / m.rows);
		if (scale < 1.0)
		{
			cv::Mat res;
			cv::resize(m, res, cv::Size(m.cols*scale, m.rows*scale));
			return res;
		}
		return m;
	}

	static inline cv::Scalar level_color(float ratio)
	{
		int deltaC = LEVEL_COLOR_MAX * ratio;
		deltaC = MAX(0, MIN(1000, deltaC));

		cv::Scalar pix;
		double* p = &pix[0];
		level_color_u(deltaC, p);
		return pix;
#if 0
		if (deltaC >= 0 && deltaC <= 255)
		{
			pix[0] = 0;
			pix[1] = deltaC;
			pix[2] = 255;
		}
		else if (deltaC > 255 && deltaC <= 510)
		{
			pix[0] = 0;
			pix[1] = 255;
			pix[2] = 255 - (deltaC - 255);
		}
		else if (deltaC > 500 && deltaC <= 765)
		{
			pix[0] = deltaC - 500;
			pix[1] = 255;
			pix[2] = 0;
		}
		else if (deltaC > 765 && deltaC <= 1020)
		{
			pix[0] = 255;
			pix[1] = 255 - (deltaC - 765);
			pix[2] = 0;
		}
		return pix;
#endif
	}

	static cv::Mat show_32F(cv::Mat m, double min_val = 0, double max_val = 0)
	{
		cv::Mat res(m.size(), CV_8UC3);
		res.setTo(0);

		if (max_val <= min_val)
			cv::minMaxLoc(m, &min_val, &max_val);

		if (max_val <= min_val)
			return cv::Mat();



		for (int i = 0; i < m.rows; i++)
		{
			uchar* pR = res.data + i*res.step[0];
			float* pM = (float*)(m.data + i*m.step[0]);
			for (int j = 0; j < m.cols; j++,pR +=3, pM++)
			{
				float value = *pM;
				if (value == 0)
					pR[0] = pR[1] = pR[2] = 0;
				{
					int ratio = 1000*(value - min_val) / (max_val - min_val);
					level_color_u(ratio, pR);
				}
			}
		}
		return res;
	}

	static cv::Mat show_16U(cv::Mat m, double min_val = 0, double max_val = 0)
	{
		cv::Mat res(m.size(), CV_8UC3);
		res.setTo(0);

		if (max_val <= min_val)
			cv::minMaxLoc(m, &min_val, &max_val);

		int mi = min_val;
		int ma = max_val;

		if (ma <= mi)
			return res;
		int d = ma - mi;

		int nr = m.rows;
		int nc = m.cols;
		if (m.isContinuous() && res.isContinuous())
		{
			nr = 1;
			nc = nc*m.rows;
		}
		for (int i = 0; i < nr; i++)
		{
			uchar* pR = res.ptr<uchar>(i);
			ushort* pM = m.ptr<ushort>(i);
			for (int j = 0; j < nc; j++, pM++)
			{
				if (*pM == 0)
				{
					*(pR++) = 0;
					*(pR++) = 0;
					*(pR++) = 0;
				}
				else if (*pM == 0xFFFF)
				{
					*(pR++) = 255;
					*(pR++) = 255;
					*(pR++) = 255;
				}
				else
				{
					int deltaC = 1279 * (*pM - mi) / d;
					level_color_u(deltaC, pR);
				}
			}
		}
		return res;
	}
#ifdef WIN32

	static void show_time(cv::Mat& m, cv::Point p = cv::Point(30, 30), int t_frame = 0)
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		BYTE* pBuff = NULL;
		char temp_time[100];//用于RGB图显示系统时间
		sprintf_s(temp_time, "%4d/%02d/%02d %02d:%02d:%02d.%03d ", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cv::putText(m, temp_time, p, cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 0, 0), 2.5);
	}
#endif
};