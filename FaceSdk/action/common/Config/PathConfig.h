#pragma once
#include <map>
#include <string>
#include <iostream>
#include "ReadIni.h"
class PathConfig
{
public:
	PathConfig() {};
	PathConfig(const char* file)
	{
		load(file);
	};
	void load(const char* file)
	{
		m_szFile = file;
		m_pathMap = ReadIni::parse_section("PATH", file);
	};
	std::string getPath(std::string name, bool bFull = true)
	{
		auto it = m_pathMap.find(name);
		if (it != m_pathMap.end())
		{
			std::string res = it->second;
			if (res.empty())
				return res;
			if (bFull)
			{
				if (res.find_first_of('/\\') == -1 || res.find_first_of('.') == 0)
				{
					res = m_szFile.substr(0, m_szFile.find_last_of("/\\") + 1) + res;
				}
			}
			return res;
		}
		return std::string();
	};
	std::map<std::string, std::string> m_pathMap;
	std::string m_szFile;
};


inline std::ostream& operator<<(std::ostream& ofs, const PathConfig& config)
{
	ofs << "[PATH]\n";
	for (auto it = config.m_pathMap.begin(); it != config.m_pathMap.end(); it++)
	{
		ofs << it->first << "=" << it->second << std::endl;
	}
	return ofs;
}