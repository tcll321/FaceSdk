#pragma once

#include "./config/RoomConfig.h"
#include "./config/ColorCameraConfig.h"
#include "./config/DetectionConfig.h"

class AllConfig
{
public:
	void load(const char* folder)
	{
		std::string szFolder = folder?folder:".";
		szFolder += "/";
		m_roomConfig.load((szFolder + "room_config.txt").c_str());
		m_shelfConfig.load((szFolder + "room_config.txt").c_str());
		m_colorCameraConfig.load((szFolder + "color_camera.txt").c_str());
		m_depthCameraConfig.load((szFolder + "depth_camera.txt").c_str());
		m_yoloConfig.load((szFolder + "yolo_config.txt").c_str());
	}

	RoomConfig m_roomConfig;
	ShelfGroupConfig m_shelfConfig;
	ColorCameraGroupConfig m_colorCameraConfig;
	DepthCameraGroupConfig m_depthCameraConfig;
	YoloConfig m_yoloConfig;
};