#pragma once
#include <string>
#include "ReadIni.h"

class ServerConfig
{
public:
	ServerConfig() {};
	void load(const char* file)
	{
		auto res = ReadIni::parse_section("SERVER", file);
		szAddr = res["address"];
		port = atoi(res["port"].c_str());
		if (port <= 0)
			port = 1920;
	}
	std::string szAddr;
	int port;
};