#pragma once

#include "BasicConfig.h"
#include <sstream>

class ColorCameraList:public BasicConfig
{
public:
	bool load(const char* file, const char* title)
	{
		szTitle = title;
		load_vec(file, title, "names", names);
		return true;
	}
	std::vector<std::string> names;
};

inline std::ostream& operator<<(std::ostream& s, const ColorCameraList& ll)
{
	s << "[" << ll.szTitle << "]" << std::endl;
	s << "names = ";
	for (int i = 0; i < ll.names.size(); i++)
	{
		s << ll.names[i];
		if (i != ll.names.size() - 1)
			s << ",";
	}
	s << std::endl;
	return s;
}

class ColorCameraConfig:public BasicConfig
{
public:
	bool load(const char* file, const char* title)
	{
		szTitle = title;
		ReadIni::load_string(title, "name", 0, file, szName);
		if (szName.empty())
		{
			szName = szTitle.substr(szTitle.find_first_of("_") + 1);
		}
		ReadIni::load_param<int>(title, "channel", channel, file);
		ReadIni::load_string(title, "path", 0, file, path);
		if (path.empty())
			path = std::to_string(channel);
		ReadIni::load_string(title, "map", 0, file, mapFile);
		load_size(file, title, "size", size);
		load_rect(file, title, "cutRect", cutRect);

		load_point3(file, title, "pos", pos);
		ReadIni::load_param<int>(title, "direction", direction, file);
		ReadIni::load_param<float>(title, "center_line_ratio", center_line_ratio, file);
		ReadIni::load_param<float>(title, "center_height_ratio", center_height_ratio, file);

		ReadIni::load_param<float>(title, "k1", k1, file);
		ReadIni::load_param<float>(title, "k2", k2, file);
		ReadIni::load_param<float>(title, "cx", cx, file);
		ReadIni::load_param<float>(title, "cy", cy, file);
		ReadIni::load_param<float>(title, "fx", fx, file);
		ReadIni::load_param<float>(title, "fy", fy, file);
		
		std::string szRod;
		ReadIni::load_string(title, "rot", 0, file, szRod);
		sscanf(szRod.c_str(), "%f,%f,%f", &rot.x, &rot.y, &rot.z);

		return true;
	}
	std::string szName;
	int channel = -1;
	std::string path;
	std::string mapFile;
	cv::Size size = cv::Size(640, 480);
	cv::Rect cutRect = cv::Rect(0,0,640, 480);
	cv::Point3f pos;
	int direction = 0; //N 0, E 1, S 2, W 3
	float center_line_ratio = 0.5;
	float center_height_ratio = 0.25;

	float k1, k2;
	float cx, cy, fx, fy;
	cv::Point3f rot;
};

inline std::ostream& operator<<(std::ostream& s, const ColorCameraConfig& cfg)
{
	s << "[" << cfg.szTitle << "]" << std::endl;
	s << "name = " << cfg.szName << std::endl;
	s << "channel = " << cfg.channel << std::endl;
	s << "map = " << cfg.mapFile << std::endl;
	s << "size = " << cfg.size.width << "," << cfg.size.height << std::endl;
	s << "cutRect = " << cfg.cutRect.x<<","<<cfg.cutRect.y<<","<<cfg.cutRect.width << "," << cfg.cutRect.height << std::endl;
	s << "pos = " << cfg.pos.x << "," << cfg.pos.y << "," << cfg.pos.z << std::endl;
	s << "direction = " << cfg.direction << std::endl;
	s << "path =" << cfg.path << std::endl;
	s << "center_line_ratio = " << cfg.center_line_ratio << std::endl;
	s << "center_height_ratio = " << cfg.center_height_ratio << std::endl;
	
	s << "k1 = " << cfg.k1 << std::endl;
	s << "k2 = " << cfg.k2 << std::endl;
	s << "cx = " << cfg.cx << std::endl;
	s << "cy = " << cfg.cy << std::endl;
	s << "fx = " << cfg.fx << std::endl;
	s << "fy = " << cfg.fy << std::endl;
	s << "rot = " << cfg.rot.x << "," << cfg.rot.y << "," << cfg.rot.z << std::endl;

	return s;
}

class ColorCameraGroupConfig
{
public:
	void load(const char* file)
	{
		ColorCameraList ll;
		ll.load(file, "CAMERA_LIST");
		for (int i = 0; i < ll.names.size(); i++)
		{
			std::string szT = std::string("CAMERA_") + ll.names[i];
			ColorCameraConfig cfg;
			cfg.load(file, szT.c_str());
			vConfig.push_back(cfg);
		}
		for (int i = 0; i < vConfig.size(); i++)
		{
			if (vConfig[i].channel >= 0)
				channelMap[vConfig[i].channel] = vConfig[i].szName;
		}
	};
	std::vector<ColorCameraConfig> vConfig;
	std::map<int, std::string> channelMap;
};

inline std::ostream& operator<<(std::ostream& s, const ColorCameraGroupConfig& cfg)
{
	ColorCameraList ll;
	ll.szTitle = "CAMERA_LIST";
	for (int i = 0; i < cfg.vConfig.size(); i++)
	{
		ll.names.push_back(cfg.vConfig[i].szName);
	}
	s << ll;
	for (int i = 0; i < cfg.vConfig.size(); i++)
	{
		s << cfg.vConfig[i];
	}
	return s;
}