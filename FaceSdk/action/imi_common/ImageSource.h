#pragma once
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include "ListFiles.h"
#include "DepthDataReader.h"

#ifndef USE_A100
#define USE_A100 0
#endif
#if USE_A100
#include "ImiImageSource.h"
//#elif USE_A100M
//#include "ImiImageSource_m.h"
#endif

#define SRC_MODE_STREAM "stream"
#define SRC_MODE_FILE "file"
#define SRC_MODE_LIST "list"
#define SRC_MODE_FOLDER "folder"

#define SRC_MODE_A100 "a100"   // src = 1 color, 2 depth, 4 ir
#define SRC_MODE_A100M "a100m"

class ImageSource
{
protected:
	static int compare_pos(const std::string & s1, const std::string & s2)
	{
		return s1.substr(0, -3) < s2.substr(0, -3);
	}
	static int compare_size(const std::string& s1, const std::string &s2)
	{
		return s1.length() < s2.length();
	}
public:
	ImageSource() {};
	ImageSource(std::string type, std::string src = "", std::string path = "0", bool bLoop = false, std::string suffix = "") :m_index(-1), m_type(type), m_bLoop(bLoop), m_suffix(suffix)
	{
		if (m_type == SRC_MODE_STREAM)
		{
			if (src.size() <= 1)
			{
				int id = src.empty() ? 0 : atoi(src.c_str());
				m_cap.open(id);
			}
			else
			{
				m_cap.open(src);
			}
		}
		else if (m_type == SRC_MODE_FILE)
		{
			m_files.push_back(path.empty() ? src : (path + "/" + src));
		}
		else if (m_type == SRC_MODE_LIST)
		{
			std::ifstream ifs(src);
			if (!ifs.is_open())
				return;
			while (!ifs.eof())
			{
				std::string line;
				ifs >> line;
				if(!line.empty())
					m_files.push_back(path.empty() ? line : (path + "/" + line));
			}
		}
		else if (m_type == SRC_MODE_FOLDER)
		{
			if (suffix.empty())
				ListFiles::process(path, "tmp.lst", true);
			else
				ListFiles::process(path, "tmp.lst", true, suffix);
			std::ifstream ifs("tmp.lst", std::ios_base::binary);
			if (!ifs.is_open())
				return;
			while (!ifs.eof())
			{
				std::string line;
				ifs >> line;
				if (!line.empty())
					m_files.push_back(path.empty() ? src : (path + "/" + line));
			}
		}
#if USE_A100
		else if (m_type == SRC_MODE_A100)
		{
			if(!src.empty())
				pA100.open(atoi(path.c_str()), atoi(src.c_str()), 0, 0);
		}
		else if (m_type == SRC_MODE_A100M)
		{
			if (!src.empty())
				pA100.open(atoi(path.c_str()), atoi(src.c_str()), 0, 1);
		}
#endif
#if USE_A100M
		else if (m_type == SRC_MODE_A100M)
		{
			if(!src.empty())
				pA100.open(atoi(path.c_str()), atoi(src.c_str()));
		}
#endif

		if (!m_files.empty())
		{
			std::stable_sort(m_files.begin(), m_files.end(), compare_pos);
			std::stable_sort(m_files.begin(), m_files.end(), compare_size);
		}
	};

	int setSize(int flag, cv::Size sz)
	{
#if USE_A100M || USE_A100
		pA100.setSize(flag, sz);
#endif
		return 0;
	}
	bool prev(cv::Mat & res, std::string& name, int flag = 0)
	{
		if (m_type == SRC_MODE_FOLDER || m_type == SRC_MODE_FILE || m_type == SRC_MODE_LIST)
		{
			if (m_files.empty())
				return false;

			m_index--;
			if (m_index < 0 && !m_bLoop)
			{
				m_index = -1;
				return false;
			}

			m_index = m_index%m_files.size();
			std::string file = m_files[m_index];
			name = file;

			if (m_suffix == ".dat")
			{
				cv::Mat image = DepthDataReader::read(file, 640, 480);
#if 0
				std::ifstream ifs(file, std::ios_base::binary);
				if (!ifs.is_open())
					return false;
				cv::Mat image(h, w, CV_16UC1);
				image.setTo(0);
				ifs.read((char*)image.data, image.step[0] * image.rows);
#endif
				image.copyTo(res);
			}
			//if (m_suffix.empty())
			else
				res = cv::imread(file);
	
			return !res.empty();
		}
		else
		{
			return false;
		}
	}
	bool next(cv::Mat& res, std::string& name, int flag = 0, void* pEx = 0)
	{
		m_index++;
		if (m_type == SRC_MODE_STREAM)
		{
			name = std::to_string(m_index);
			if (!m_cap.isOpened())
				return false;
			else if (m_cap.grab())
			{
				return m_cap.retrieve(res);
			}
		}
		else if (m_type == SRC_MODE_A100 || m_type == SRC_MODE_A100M)
		{
#if USE_A100
			return pA100.next(res, name, flag, pEx);
#else
			return false;
#endif
		}
		else
		{
			if (m_files.empty())
				return false;
			if (m_index >= m_files.size() && !m_bLoop)
			{
				m_index = m_files.size();
				return false;
			}
			m_index = m_index%m_files.size();
			std::string file = m_files[m_index];
			name = file;
			if (m_suffix == ".dat")
			{
				cv::Mat image = DepthDataReader::read(file, 640, 480);
#if 0
				std::ifstream ifs(file, std::ios_base::binary);
				if (!ifs.is_open())
					return false;
				cv::Mat image(h, w, CV_16UC1);
				image.setTo(0);
				ifs.read((char*)image.data, image.step[0] * image.rows);
#endif
				image.copyTo(res);
			}
			else// if (m_suffix == ".png")
			{
				res = cv::imread(file, CV_LOAD_IMAGE_UNCHANGED);
			}
			//else
			//	res = cv::imread(file);
			return !res.empty();
		}
		return false;
	};

	std::string m_type;
	std::string m_suffix;
	bool m_bLoop;
	int m_index;
	cv::VideoCapture m_cap;
	std::vector<std::string> m_files;
#if USE_A100 || USE_A100M
	ImiImageSource pA100;
#endif
};