#pragma once

#include "BasicConfig.h"
#include "ShelfConfig.h"
class RoomConfig :public BasicConfig
{
public:
	bool load(const char* file, const char* title = "ROOM")
	{
		szTitle = title;
		load_size(file, title, "room_size", room_size);
		load_rect(file, title, "door_area", door_area);
		ReadIni::load_param<float>(title, "scale", scale, file);
		return true;
	}

	cv::Size room_size = cv::Size(4000, 5000);
	cv::Rect door_area;
	float scale = 0.2;
};

static std::ostream& operator<<(std::ostream& s, const RoomConfig& cfg)
{
	s << "[" << cfg.szTitle << "]" << std::endl;
	s << "room_size =" << cfg.room_size.width << "," << cfg.room_size.width << std::endl;
	s << "door_area = " << cfg.door_area.x << "," << cfg.door_area.y << "," << cfg.door_area.width << "," << cfg.door_area.height << std::endl;
	s << "scale = " << cfg.scale << std::endl;
	return s;
}

class ShelfInfoConfig :public BasicConfig
{
public:
	bool load(const char* file, const char* title)
	{
		
	}
	
	cv::Size size;
	int ground = 0;
	int floors = 0;
	int inner_line = 0, outer_line = 30;
};