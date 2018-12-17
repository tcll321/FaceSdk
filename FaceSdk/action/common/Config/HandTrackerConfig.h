#pragma once

#include "readini.h"
#include "basicconfig.h"
class HandTrackerConfig:public BasicConfig
{
public:
	void load(const char* file)
	{
		ReadIni::load_param("HANDTRACKER", "inside", inside, file);
		ReadIni::load_param("HANDTRACKER", "outside", outside, file);
		ReadIni::load_param("HANDTRACKER", "inside_stable", inside_stable, file);
		ReadIni::load_param("HANDTRACKER", "outside_stable", outside_stable, file);
		ReadIni::load_param("HANDTRACKER", "stable_count", stable_count, file);
		ReadIni::load_param("HANDTRACKER", "lost_count", lost_count, file);
	};

	float inside;
	float outside;
	float inside_stable;
	float outside_stable;

	int stable_count;
	int lost_count;
};


static std::ostream& operator<<(std::ostream& s, const HandTrackerConfig& config)
{
	s << "[HANDTRACKER]" << std::endl;
	s << "inside = " << config.inside << std::endl;
	s << "outside = " << config.outside << std::endl;
	s << "inside_stable = " << config.inside_stable << std::endl;
	s << "outside_stable = " << config.outside_stable << std::endl;
	s << "stable_count = " << config.stable_count << std::endl;
	s << "lost_count" << config.lost_count << std::endl;
	return s;
}