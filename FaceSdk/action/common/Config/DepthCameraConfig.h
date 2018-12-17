#pragma once

#include "BasicConfig.h"


enum {
	DC_TYPE_ROOF = 0,
	DC_TYPE_SHELL = 1
};


class DepthCameraList :public BasicConfig
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

inline std::ostream& operator<<(std::ostream& s, const DepthCameraList& ll)
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

class DepthCameraConfig:public BasicConfig
{
public:
	DepthCameraConfig() {};
	bool load(const char* file, const char* title)
	{
		szTitle = title;
		ReadIni::load_param<int>(title, "type", type, file);
		ReadIni::load_param<int>(title, "channel", channel, file);
		ReadIni::load_string(title, "path", 0, file, path);
		if (path.empty())
			path = std::to_string(channel);

		ReadIni::load_string(title, "name", 0, file, name);
		if (name.empty())
			name = szTitle.substr(szTitle.find_first_of("_") + 1);
			//name = std::to_string(channel);

		load_point3(file, title, "pos", pos);
		load_point3(file, title, "center", center);

		load_size(file, title, "size", size);

		ReadIni::load_param<int>(title, "direction", direction, file);
		//ReadIni::load_param<float>(title, "angle", angle, file);
		ReadIni::load_param<float>(title, "offsetX", offsetX, file);
		ReadIni::load_param<float>(title, "offsetZ", offsetZ, file);

		ReadIni::load_param<int>(title, "left", left, file);
		ReadIni::load_param<int>(title, "right", right, file);

		ReadIni::load_param<int>(title, "live", bLive, file);
		ReadIni::load_param<int>(title, "skeleton", bSkeleton, file);
		ReadIni::load_param<int>(title, "png", bPng, file);
		ReadIni::load_param<int>(title, "fps", fps, file);

		std::vector<float> v;
		load_vec(file, title, "gravity", v);
		if (v.size() >= 3)
		{
			gravity = cv::Vec3f(v[0], v[1], v[2]);
		}
		
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
	};

	int type = 0; //0 a100, 1 a100m

	int channel = 0;
	std::string path;
	std::string name;

	cv::Point3f pos;
	cv::Point3f center;

	cv::Size size = cv::Size(640, 480);

	cv::Vec3f gravity;

	int direction = 0;
	//float angle = 0;
	float offsetX = 0;
	float offsetZ = 0;

	int left = 0;
	int right = 0;

	int bLive = 1;
	int bSkeleton = 1;
	int bPng = 0;
	int fps = 20;

	float k1, k2;
	float cx, cy, fx, fy;
	cv::Point3f rot;
};

inline std::ostream& operator<<(std::ostream& s, const DepthCameraConfig& cfg)
{
	s << "[" << cfg.szTitle << "]" << std::endl;

	s << "type = " << cfg.type << std::endl;
	s << "channel = " << cfg.channel << std::endl;
	s << "path = " << cfg.path << std::endl;
	s << "name = " << cfg.name << std::endl;

	s << "pos = " << cfg.pos.x << "," << cfg.pos.y << "," << cfg.pos.z << std::endl;

	s << "center = " << cfg.center.x << "," << cfg.center.y << "," << cfg.center.z << std::endl;
	s << "direction = " << cfg.direction << std::endl;
	s << "size = " << cfg.size.width << "," << cfg.size.height << std::endl;

	s << "gravity = " << cfg.gravity[0] << "," << cfg.gravity[1] << "," << cfg.gravity[2] << std::endl;
	//s << "angle = " << cfg.angle << std::endl;
	s << "offsetX = " << cfg.offsetX << std::endl;
	s << "offsetZ = " << cfg.offsetZ << std::endl;

	s << "left = " << cfg.left << std::endl;
	s << "right = " << cfg.right << std::endl;

	s << "live = " << cfg.bLive << std::endl;
	s << "skeleton = " << cfg.bSkeleton << std::endl;
	s << "png = " << cfg.bPng << std::endl;
	s << "fps = " << cfg.fps << std::endl;

	s << "k1 = " << cfg.k1 << std::endl;
	s << "k2 = " << cfg.k2 << std::endl;
	s << "cx = " << cfg.cx << std::endl;
	s << "cy = " << cfg.cy << std::endl;
	s << "fx = " << cfg.fx << std::endl;
	s << "fy = " << cfg.fy << std::endl;
	s << "rot = " << cfg.rot.x << "," << cfg.rot.y << "," << cfg.rot.z << std::endl;

	return s;
}


class DepthCameraGroupConfig
{
public:
	void load(const char* file)
	{
		DepthCameraList ll;
		ll.load(file, "CAMERA_LIST");
		for (int i = 0; i < ll.names.size(); i++)
		{
			std::string szT = std::string("CAMERA_") + ll.names[i];
			DepthCameraConfig cfg;
			cfg.load(file, szT.c_str());
			vConfig.push_back(cfg);
		}
		for (int i = 0; i < vConfig.size(); i++)
		{
			if (vConfig[i].channel >= 0)
				channelMap[vConfig[i].channel] = vConfig[i].name;
		}
	};
	std::vector<DepthCameraConfig> vConfig;
	std::map<int, std::string> channelMap;
};

inline std::ostream& operator<<(std::ostream& s, const DepthCameraGroupConfig& cfg)
{
	DepthCameraList ll;
	ll.szTitle = "CAMERA_LIST";
	for (int i = 0; i < cfg.vConfig.size(); i++)
	{
		ll.names.push_back(cfg.vConfig[i].name);
	}
	s << ll;
	for (int i = 0; i < cfg.vConfig.size(); i++)
	{
		s << cfg.vConfig[i];
	}
	return s;
}