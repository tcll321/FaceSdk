#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

#define REGRESS_LINE 0
#define REGRESS_PARABOLA 1

class Regression
{
public:
	static int regress_iterate(int type, std::vector<cv::Point> vp, std::vector<int>& vlabel, std::vector<float>& weight, std::vector<float>& param, float div_thresh = 30.f, float l2_thresh = -1e-3, bool bNormal = false)
	{
		if (vp.size() <= 8)
			return -1;
		int size = vp.size();
		float div = 1000;
		float sign = -1;
		while (size > 8 && (div > div_thresh || type == REGRESS_PARABOLA && sign < l2_thresh))
		{
			int ret = 0;
			if (type == REGRESS_LINE)
			{
				ret = regress_line(vp, vlabel, param);
				
			}
			else if (type == REGRESS_PARABOLA)
			{
				ret = regress_parabola(vp, vlabel, param, weight, bNormal);
			}
			if (ret || param.empty())
				return -2;

			div = 0;
			for (int i = 0; i < vp.size(); i++)
			{
				if (!vlabel[i])
					continue;
				float d = fabs(vp[i].y - eval(type, vp[i].x, param));
				div = (std::max)(div, d);
				if (d > div_thresh)
					vlabel[i] = 0;
			}

			sign = param[0];
			if (type == REGRESS_PARABOLA && sign < l2_thresh)
			{
				int minId = -1;
				float maxY = 0;
				for (int i = 0; i < vp.size(); i++)
				{
					if (vlabel[i] && vp[i].y > maxY)
					{
						maxY = vp[i].y;
						minId = i;
					}
				}
				if(minId >= 0)
					vlabel[minId] = 0;
			}
			size = 0;
			for (int i = 0; i < vlabel.size(); i++)
			{
				if (vlabel[i])
					size++;
			}

		}

		float mean_dist = 0;
		int mean_count = 0;
		for (int i = 0; i < vlabel.size(); i++)
		{
			if (!vlabel[i])
				continue;
			mean_dist += fabs(vp[i].y - eval(type, vp[i].x, param));
			mean_count++;
		}

		mean_dist /= mean_count;

		for (int i = 0; i < vlabel.size(); i++)
		{
			if (vlabel[i] && fabs(vp[i].y - eval(type, vp[i].x, param)) > mean_dist * 3)
				vlabel[i] = 0;
		}

		return 0;
	}

	static int regress_line(std::vector<cv::Point> vp, std::vector<int>& vlabel, std::vector<float>& param, bool bNormal = false)
	{
		param.clear();
		if (vp.size() <= 8)
			return -1;
		float k, b;

		double suma = 0;
		double sumb = 0;
		double sumab = 0;
		double sum1 = 0;
		double sum2 = 0;
		int t = 0;
		for (int i = 0; i < vp.size(); i++) {
			if (!vlabel[i])
				continue;
			suma += vp[i].x * vp[i].y;
			sum1 += vp[i].x;
			sum2 += vp[i].y;
			t++;
		}

		sumb -= (suma*sum1);
		// System.out.println(sumb+" "+sum1+" "+sum2);  
		suma *= t;
		sumab -= sum1*sum1;
		suma -= sum1*sum2;
		sum1 = 0;
		for (int i = 0; i < t; i++) {
			sum1 += vp[i].x * vp[i].x;
		}
		//System.out.println(sumb+" "+sum1+" "+sum2);  
		sumb += sum1*sum2;
		sumab += t*sum1;

		if (fabs(sumab) < 1e-6)
			return -2;

		k = suma / sumab;
		b = sumb / sumab;
		param.push_back(k);
		param.push_back(b);
		
		return 0;
	}

	static int regress_parabola(std::vector<cv::Point> vp, std::vector<int>& vlabel, std::vector<float>& param, std::vector<float> &weight, bool bNormal = false)
	{
		int vlabel_count = 0;
		for (int i = 0; i < vlabel.size(); i++)
		{
			if (vlabel[i])
				vlabel_count++;
		}
		if (vlabel_count <= 5)
			return -1;

		cv::Mat A(vlabel_count, 3, CV_32FC1);
		cv::Mat b(vlabel_count, 1, CV_32FC1);

		int count = 0;
		for (int i = 0; i < vp.size(); i++)
		{
			if(!vlabel[i])
				continue;
			float w = weight.empty() ? 1 : weight[i];
		
			float scale = bNormal ? (std::max)(1.0, pow(1.0*vp[i].x, 2)) : 1.0;
			A.at<float>(count, 0) = vp[i].x*vp[i].x*w / scale;
			A.at<float>(count, 1) = vp[i].x*w / scale;
			A.at<float>(count, 2) = 1 * w / scale;
			b.at<float>(count) = vp[i].y*w / scale;
			count++;
		}


		cv::Mat X;
		cv::solve(A, b, X, cv::DECOMP_SVD);
		param.push_back(X.at<float>(0));
		param.push_back(X.at<float>(1));
		param.push_back(X.at<float>(2));

		return 0;
	}

	static int regress_parabola_2(std::vector<cv::Point> vp, std::vector<int>& vlabel, std::vector<float>& param)
	{
		param.clear();
		double x1 = 0, x2 = 0, x3 = 0, x4 = 0;
		double x0y = 0, x1y = 0, x2y = 0;

		for (int i = 0; i < vp.size(); i++)
		{
			x1 += vp[i].x;
			x2 += vp[i].x * vp[i].x;
			x3 += pow(vp[i].x, 3);
			x4 += pow(vp[i].x, 4);

			x0y += vp[i].y;
			x1y += vp[i].x * vp[i].y;
			x2y += vp[i].x * vp[i].x *vp[i].y;
		}
		cv::Mat_<double> A(3, 3);
		A.at<double>(0, 0) = vp.size();	A.at<double>(0, 1) = x1;		A.at<double>(0, 2) = x2;
		A.at<double>(1, 0) = x1;		A.at<double>(1, 1) = x2;		A.at<double>(1, 2) = x3;
		A.at<double>(2, 0) = x2;		A.at<double>(2, 1) = x3;		A.at<double>(2, 2) = x4;
		cv::Mat_<double> b(3, 1);
		b.at<double>(0) = x0y;			b.at<double>(1) = x1y;			b.at<double>(2) = x2y;

		cv::Mat X;
		cv::solve(A, b, X, cv::DECOMP_SVD);

		param.push_back(X.at<double>(2));
		param.push_back(X.at<double>(1));
		param.push_back(X.at<double>(0));

		return 0;
	}

	static float eval(int type, float x, std::vector<float>& param)
	{
		if (type == REGRESS_LINE)
			return x * param[0] + param[1];
		else if (type == REGRESS_PARABOLA)
			return x*x*param[0] + x*param[1] + param[2];
		return 0;
	}

	static int check_pair(int type, std::vector<float>& p1, std::vector<float>& p2)
	{
		if (type == REGRESS_LINE && abs(atanf(p1[0]) - atanf(p2[0])) >= 0.05)
			return 1;
		else if (type == REGRESS_PARABOLA && abs(p1[0] - p2[0]) >= 0.1)
			return 1;

		return 0;
	}

	static std::vector<float> group_weight(cv::Point center, std::vector<cv::Point> &vp, std::vector<int> &label, float dist_thresh = 12, float power = 0, float cut = 0)
	{
		std::vector<float> weight(label.size(), 1);
		int id = 0;
		int id_count = 1;
		for (int i = 1; i < vp.size(); i++)
		{
			//std::cout << norm(vp[i] - vp[i - 1]) << ",   ";
			if (norm(vp[i] - vp[i - 1]) < dist_thresh)
			{
				id_count++;
			}
			else
			{
				for (int k = id; k < i; k++)
					weight[k] = id_count;
				id = i;
				id_count = 1;
			}

			if (i == vp.size() - 1)
			{
				for (int k = id; k <= i; k++)
					weight[k] = id_count;
				id = i;
				id_count = 1;
			}
		}

		for (int i = 0; i < weight.size(); i++)
		{
			float valid = 0;
			if (vp[i].y < center.y)
			{
				valid = exp(-pow(vp[i].x - center.x, 2) / 10000 - pow(vp[i].y - center.y, 2) / 2000);
			}
			else
			{
				valid = exp(-pow(vp[i].x - center.x, 2) / 12000 - pow(vp[i].y - center.y, 2) / 7000);
			}
			weight[i] *= valid;
		}

		//std::cout << std::endl;
		for (int i = 0; i < weight.size(); i++)
		{
			if (weight[i] == 1)
				label[i] = 0;
			if (cut)
				weight[i] = (std::min)(weight[i], cut);

			if(power)
				weight[i] = pow(weight[i], power);

			//std::cout << weight[i] << ",";
		}
		//std::cout << std::endl;
		return weight;
	}

	static cv::Mat line_mask(cv::Size sz, int type, std::vector<float> param, bool bDown = true, float row_ratio = 0, float col_ratio = 0)
	{
		cv::Mat mask(sz, CV_8UC1);
		mask.setTo(0);
		for (int x = col_ratio*sz.width; x < (1-col_ratio)*sz.width; x++)
		{
			float y = eval(type, x, param);
			if(y < 0 || y >= sz.height)
				continue;
			float y2 = 0;
			if (bDown)
			{
				y2 = row_ratio > 0 ? (y + sz.height*row_ratio) : sz.height;
			}
			else
			{
				y2 = row_ratio > 0 ? (y - sz.height*row_ratio) : 0;
			}
			cv::line(mask, cv::Point(x, y), cv::Point(x, y2), cv::Scalar(255, 255, 255));
		}
		return mask;
	}
};