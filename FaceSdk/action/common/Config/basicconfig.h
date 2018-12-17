#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include "ReadIni.h"
#include <sstream>
#include <ostream>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
class BasicConfig
{
public:
	virtual bool load(const char* file, const char* title)
	{
		szTitle = title;
		return true;
	};

	static bool load_string(const char* file, const char* title, const char* tag, std::string& str)
	{
		ReadIni::load_string(title, tag, 0, file, str);
		return !str.empty();
	};
	static bool load_size(const char* file, const char* title, const char* tag, cv::Size& sz)
	{
		std::string str;
		bool bRet = load_string(file, title, tag, str);
		if (!bRet)
			return false;

		int count = sscanf(str.c_str(), "%d,%d", &sz.width, &sz.height);
		return count == 2;
	};
	static bool load_rect(const char* file, const char* title, const char* tag, cv::Rect& rect)
	{
		std::string str;
		bool bRet = load_string(file, title, tag, str);
		if (!bRet)
			return false;

		int count = sscanf(str.c_str(), "%d,%d,%d,%d", &rect.x, &rect.y, &rect.width, &rect.height);
		return count == 4;
	};
	template <typename T>
	static bool load_point(const char* file, const char* title, const char* tag, cv::Point& p)
	{
		std::string str;
		bool bRet = load_string(file, title, tag, str);
		if (!bRet)
			return false;

		int count = sscanf(str.c_str(), "%d,%d", &p.x, &p.y);
		return count == 2;
	};

	static bool load_point3(const char* file, const char* title, const char* tag, cv::Point3f& p)
	{
		std::string str;
		bool bRet = load_string(file, title, tag, str);
		if (!bRet)
			return false;

		int count = sscanf(str.c_str(), "%f,%f,%f", &p.x, &p.y, &p.z);
		return count == 3;
	};

	template <typename T>
	static bool load_scalar(const char* file, const char* title, const char* tag, cv::Scalar& s)
	{
		std::string str;
		bool bRet = load_string(file, title, tag, str);
		if (!bRet)
			return false;
		int count = sscanf(str.c_str(), "%f,%f,%f,%f", &s[0], &s[1], &s[2], &s[3]);
		return count >= 2;
	};

	static bool load_vec(const char* file, const char* title, const char* tag, std::vector<float>& val)
	{
		std::string str;
		bool bRet = load_string(file, title, tag, str);
		if (!bRet)
			return false;

		int count = 0;
		if (1)
		{
			std::stringstream ss(str);
			std::string item;
			while (std::getline(ss, item, ',') && !item.empty())
			{
				val.push_back(atof(item.c_str()));
				count++;
			}
		}
		return count > 0;
	};

	static bool load_vec(const char* file, const char* title, const char* tag, std::vector<std::string>& val)
	{
		std::string str;
		bool bRet = load_string(file, title, tag, str);
		if (!bRet)
			return false;

		int count = 0;
		if (1)
		{
			std::stringstream ss(str);
			std::string item;
			while (std::getline(ss, item, ',') && !item.empty())
			{
				item = item.substr(item.find_first_not_of(" \t"));
				item = item.substr(0, item.find_last_not_of(" \t") + 1);
				val.push_back(item);
				count++;
			}
		}
		return count > 0;
	};


	std::string szTitle;
};

enum {
	DIR_NORTH = 0,
	DIR_EAST = 1,
	DIR_SOUTH = 2,
	DIR_WEST = 3,
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif