#pragma once

#include "BasicConfig.h"

class DepthTrackerConfig :public BasicConfig
{
public:
	bool load(const char* file, const char* title = "DEPTH_TRACKER")
	{
		szTitle = title;
		ReadIni::load_param<int>(title, "codebook_size", codebook_size, file);
		ReadIni::load_param<float>(title, "mesh_ratio", mesh_ratio, file);
		ReadIni::load_param<float>(title, "mesh_ratio_inv", mesh_ratio_inv, file);
		return true;
	};

	float mesh_ratio = 0.1;
	float mesh_ratio_inv = 0.02;
	int codebook_size = 5;
};

static std::ostream& operator<<(std::ostream& s, const DepthTrackerConfig& cfg)
{
	s << "[" << cfg.szTitle << "]" << std::endl;
	s << "codebook_size = " << cfg.codebook_size << std::endl;
	s << "mesh_ratio = " << cfg.mesh_ratio << std::endl;
	s << "mesh_ratio_inv =" << cfg.mesh_ratio_inv << std::endl;
	return s;
}