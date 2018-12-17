#pragma once

#include <opencv2/opencv.hpp>

class MaskProcess
{
public:
	static void merge(cv::Mat src, cv::Mat mask, cv::Mat& dst)
	{
		if (src.channels() == 1)
		{
			cv::Mat color;
			cv::cvtColor(src, color, CV_GRAY2BGR);
			src = color;
		}
		dst.create(src.size(), CV_8UC3);
		for (int i = 0; i < src.rows; i++)
		{
			uchar* pSrc = src.data + i*src.step[0];
			uchar* pDst = dst.data + i*dst.step[0];
			uchar* pMask = mask.data + i*mask.step[0];
			for (int j = 0; j < src.cols; j++, pMask++, pSrc += 3, pDst += 3)
			{
				if (pMask[0])
				{
					pDst[0] = pSrc[0] * 0.6;
					pDst[1] = pSrc[1] * 0.5 + 127;
					pDst[2] = pSrc[2] * 0.6;
				}
				else
				{
					pDst[0] = pSrc[0];
					pDst[1] = pSrc[1];
					pDst[2] = pSrc[2];
				}
			}
		}
	}

	static cv::Mat generate_gaussian(cv::Size sz, cv::Point center, double deltaX = 15000, double deltaY = 8000)
	{
		cv::Mat_<float> mask(sz);
		mask.setTo(0);

		if (deltaX <= 0)
			deltaX = sz.width*sz.width / 2;
		if (deltaY <= 0)
			deltaY = sz.height*sz.height / 4;

		float dY = deltaY, dX = deltaX;
		for (int i = 0; i < sz.height; i++)
		{
			float* pMask = (float*)(mask.data + i*mask.step[0]);
			for (int j = 0; j < sz.width; j++, pMask++)
			{
				*pMask = expf(-(i - center.y)*(i - center.y) / dY - (j - center.x)*(j - center.x)/ dX);
			}
		}
		return mask;
	}

	static float otsu_thresh(double* Histogram, float & ratio, float rMin = 0, float rMax = 0)
	{
		int T = 0; //Otsu算法阈值  
		double varValue = 0; //类间方差中间值保存  
		double w0 = 0; //前景像素点数所占比例  
		double w1 = 0; //背景像素点数所占比例  
		double u0 = 0; //前景平均灰度  
		double u1 = 0; //背景平均灰度  
		double minW = 0;
		float totalNum = 0;
		for (int i = 0; i < 256; i++)
		{
			totalNum += Histogram[i];
		}
		if (totalNum <= 1e-8)
			return 0;
		for (int i = 0; i < 255; i++)
		{
			//每次遍历之前初始化各变量  
			w1 = 0;       u1 = 0;       w0 = 0;       u0 = 0;
			//***********背景各分量值计算**************************  
			for (int j = 0; j <= i; j++) //背景部分各值计算  
			{
				w1 += Histogram[j];  //背景部分像素点总数  
				u1 += j*Histogram[j]; //背景部分像素总灰度和  
			}
			if (w1 == 0) //背景部分像素点数为0时退出  
			{
				continue;
			}
			u1 = u1 / w1; //背景像素平均灰度  
			w1 = w1 / totalNum; // 背景部分像素点数所占比例  
								//***********背景各分量值计算**************************  

								//***********前景各分量值计算**************************  
			for (int k = i + 1; k < 255; k++)
			{
				w0 += Histogram[k];  //前景部分像素点总数  
				u0 += k*Histogram[k]; //前景部分像素总灰度和  
			}
			if (w0 == 0) //前景部分像素点数为0时退出  
			{
				continue;
			}
			if (rMin > 0 && w1 < rMin || rMax > 0 && w1 > rMax)
				continue;

			u0 = u0 / w0; //前景像素平均灰度  
			w0 = w0 / totalNum; // 前景部分像素点数所占比例  
								//***********前景各分量值计算**************************  

								//***********类间方差计算******************************  
			double varValueI = w0*w1*(u1 - u0)*(u1 - u0); //当前类间方差计算  
			if (varValue < varValueI)
			{
				varValue = varValueI;
				T = i;
				minW = w1;
			}
		}
		ratio = minW;
		return T;
	}

	static cv::Mat masked_otsu(cv::Mat src, cv::Mat mask, cv::Mat weight, float& thresh, bool bInv,
		float rMin = 0, float rMax = 0, 
		float dark_cut = 0, float light_cut = 0
	)
	{
#if 1
		if (src.channels() == 3)
		{
			cv::Mat gray;
			cv::cvtColor(src, gray, CV_BGR2GRAY);
			src = gray;
		}
#endif
		cv::Mat res;
	
		double Histogram[256] = { 0 }; //灰度直方图，下标是灰度值，保存内容是灰度值对应的像素点总数  
	//	uchar *data = src.data;
	//	double totalNum = 0; // image.rows*image.cols; //像素总数  
												 //计算灰度直方图分布，Histogram数组下标是灰度值，保存内容是灰度值对应像素点数  

		if (!mask.empty())
		{
			for (int i = 0; i < src.rows; i++)   //为表述清晰，并没有把rows和cols单独提出来  
			{
				uchar* pSrc = src.data + i*src.step[0];
				uchar* pMask = mask.data + i*mask.step[0];
				for (int j = 0; j < src.cols; j++, pSrc++, pMask++)
				{
					
					if (*pMask)
					{
						float val = weight.empty() ? 1 : weight.at<float>(i, j);
						Histogram[*pSrc] += val;
					}
	
				}
			}
		}
		else
		{
			for (int i = 0; i < src.rows; i++)   //为表述清晰，并没有把rows和cols单独提出来  
			{
				uchar* pSrc = src.data + i*src.step[0];
				for (int j = 0; j < src.cols; j++, pSrc++)
				{
					float val = weight.empty() ? 1 : weight.at<float>(i, j);
					Histogram[*pSrc] += val;
				}
			}
		}
#if 0
		if (totalNum == 0)
		{
			res.create(src.size(), CV_8UC1);
			res.setTo(0);
			thresh = 0;
			return res;
		}
#endif
		float rr = 0;
		float r_dark = 0;
		float r_light = 1;
		float t_dark = -1, t_light = -1;
		if (dark_cut > 0)
		{
			t_dark = otsu_thresh(Histogram, r_dark, 0, dark_cut);
		}
		if (light_cut > 0)
		{
			t_light = otsu_thresh(Histogram, r_light, light_cut, 0);
		}

		float r_min = rMin > 0 ? ((r_light - r_dark)*rMin + r_dark) : r_dark;
		float r_max = rMax > 0 ? ((r_light - r_dark)*rMax + r_dark) : r_light;
		float T = otsu_thresh(Histogram, rr, r_min, r_max);

		//std::cout << "otsu r_min:" << r_min << ",r_max:" << r_max << std::endl;
	
		cv::Mat mDark, mLight;
		cv::threshold(src, res, T, 255, bInv ? CV_THRESH_BINARY_INV : CV_THRESH_BINARY);
		if (t_dark >= 0)
		{
			cv::threshold(src, mDark, t_dark, 255, CV_THRESH_BINARY);
			res &= mDark;
		}
		if (t_light >= 0)
		{
			cv::threshold(src, mLight, t_light, 255, CV_THRESH_BINARY_INV);
			res &= mLight;
		}
	
		thresh = T;
		if(!mask.empty())
			res &= mask;
		return res;
	}
};