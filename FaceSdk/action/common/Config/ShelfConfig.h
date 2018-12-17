#pragma once
#include "basicconfig.h"
#include "ColorCameraConfig.h"
#include "DepthCameraConfig.h"

class ShelfListConfig :public BasicConfig
{
public:
	bool load(const char* file, const char* title = "SHELF_LIST")
	{
		szTitle = title;
		load_vec(file, title, "names", names);
		return true;
	}
	std::vector<std::string> names;
};

inline std::ostream& operator<<(std::ostream& s, const ShelfListConfig& ll)
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

class ShelfConfig :public BasicConfig
{
public:
	bool load(const char* file, const char* title)
	{
		szTitle = title;
		ReadIni::load_string(title, "name", 0, file, name);
		if (name.empty())
			name = szTitle.substr(szTitle.find_first_of("_") + 1);
		load_rect(file, title, "pos", pos);
		ReadIni::load_param<int>(title, "direction", direction, file);

		ReadIni::load_param<int>(title, "height", height, file);
		ReadIni::load_param<int>(title, "ground", ground, file);
		ReadIni::load_param<int>(title, "layers", layers, file);

		ReadIni::load_param<int>(title, "line_inner", line_inner, file);
		ReadIni::load_param<int>(title, "line_outer", line_outer, file);

		ReadIni::load_param<int>(title, "hand_head_max", hand_head_max, file);
		ReadIni::load_param<int>(title, "hand_height_min", hand_height_min, file);
		ReadIni::load_param<int>(title, "hand_shelf_min", hand_shelf_max, file);

		load_vec(file, title, "color_camera", color_camera);
		load_vec(file, title, "depth_camera", depth_camera);

		return true;
	}
	std::string name;
	cv::Rect pos;
	int direction = 0;

	int height = 2000;
	int ground = 500;
	int layers = 3;

	int line_inner = 150;
	int line_outer = 400;

	int hand_height_min = 500;
	int hand_head_max = 700;
	int hand_shelf_max = 600;

	std::vector<std::string> color_camera;
	std::vector<std::string> depth_camera;
};

static inline std::ostream& operator<<(std::ostream& s, const ShelfConfig& cfg)
{
	s << "[" << cfg.szTitle << "]" << std::endl;
	s << "name = " << cfg.name << std::endl;
	s << "pos = " << cfg.pos.x << "," << cfg.pos.y << "," << cfg.pos.width << "," << cfg.pos.height << std::endl;
	s << "direction = " << cfg.direction << std::endl;

	s << "height = " << cfg.height << std::endl;
	s << "ground = " << cfg.ground << std::endl;
	s << "layers = " << cfg.layers << std::endl;

	s << "line_inner = " << cfg.line_inner << std::endl;
	s << "line_outer = " << cfg.line_outer << std::endl;

	s << "hand_head_max = " << cfg.hand_head_max << std::endl;
	s << "hand_height_min = " << cfg.hand_height_min << std::endl;
	s << "hand_shelf_min = " << cfg.hand_shelf_max << std::endl;

	s << "color_camera = ";
	for (int i = 0; i < cfg.color_camera.size(); i++)
	{
		s << cfg.color_camera[i];
		if (i != cfg.color_camera.size() - 1)
			s << ",";
	}
	s << std::endl;

	s << "depth_camera = ";
	for (int i = 0; i < cfg.depth_camera.size(); i++)
	{
		s << cfg.depth_camera[i];
		if (i != cfg.depth_camera.size() - 1)
			s << ",";
	}
	s << std::endl;
	return s;
}

class ShelfGroupConfig
{
public:
	void load(const char* file)
	{
		shelfList.load(file);

		for (int i = 0; i < shelfList.names.size(); i++)
		{
			std::string szS = std::string("SHELF_" + shelfList.names[i]);
			ShelfConfig cfg;
			cfg.load(file, szS.c_str());
			vShelf.push_back(cfg);
		}
	}
	ShelfListConfig shelfList;
	std::vector<ShelfConfig> vShelf;
};

static inline std::ostream & operator<<(std::ostream& s, const ShelfGroupConfig& cfg)
{
	s << cfg.shelfList;
	for (int i = 0; i < cfg.vShelf.size(); i++)
	{
		s << cfg.vShelf[i];
	}
	return s;
}