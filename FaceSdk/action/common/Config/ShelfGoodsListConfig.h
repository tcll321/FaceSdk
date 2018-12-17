#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "ShelfConfig.h"
#include "ReadIni.h"

class LayerBinConfig
{
public:
	void load(const char* file)
	{
		ReadIni::load_param("LAYER", "count", layer_count, file);
		ReadIni::load_param("LAYER", "min", layer_min, file);
		ReadIni::load_param("LAYER", "width", layer_width, file);
		ReadIni::load_param("LAYER", "height", layer_height, file);

		ReadIni::load_param("BIN", "count", bin_count, file);
	}
	//std::vector<int, cv::Point3d> bin_rect;
	int layer_count;
	int layer_min;
	int layer_width;
	int layer_height;

	//int shelf_count;
	int bin_count;
	//int bin_width;
	//int bin_height;
};

struct ShelfGoodsNode
{
	int type;
	int shelfID;
	int layerID;
	int binID;
};

static std::ostream& operator<<(std::ostream& os, ShelfGoodsNode node)
{
	os<<node.type<<","<<node.shelfID<<","<<node.layerID<<"," << node.binID << std::endl;
	return os;
}

class ShelfGoodsList
{
public:
	ShelfGoodsList() {}
	void load(const char* file)
	{
		std::ifstream ifs(file);
		if (!ifs.is_open())
			return;
		std::string szLine;
		while (std::getline(ifs, szLine))
		{
			ShelfGoodsNode node;
			int count = sscanf(szLine.c_str(), "%d,%d,%d,%d", &node.type, &node.shelfID, &node.layerID, &node.binID);
			if (count == 4)
			{
				type_map.insert(_get_uid(node));
			}
		}
	}
	void save(const char* file)
	{
		std::ofstream ofs(file);
		if (!ofs.is_open())
			return;
		for (auto it = type_map.begin(); it != type_map.end(); it++)
		{
			ShelfGoodsNode node = _get_node(*it);
			ofs << node;
		}
	}


	std::pair<int, int> _get_uid(ShelfGoodsNode node)
	{
		return std::make_pair( (node.shelfID << 16) + (node.layerID << 8) + (node.layerID), node.type);
	};
	ShelfGoodsNode _get_node(std::pair<int, int> uid)
	{
		ShelfGoodsNode node;
		node.type = uid.second;
		node.shelfID = uid.first >> 16;
		node.layerID = (uid.first >> 8) & 255;
		node.binID = uid.first & 255;
		return node;
	};
	
	std::map<int, int> type_map;
};


class ShelfLayerBin
{
public:
	ShelfLayerBin(ShelfGroupConfig sC, LayerBinConfig lC) {
	
	};

	int getNearestBin(cv::Point3f hand_pos);
	cv::Point3f getPos(int bin_id);
	std::vector<int> getNeighbor(int bin_id);
};