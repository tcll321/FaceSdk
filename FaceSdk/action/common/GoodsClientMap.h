#pragma once
#include <opencv2/opencv.hpp>
#include "BasicStructs.h"
#include "../HandTracker/HandTrackerData.h"
typedef enum ALARM_TYPE //state type
{
	ALARM_DEFAULT = 0,
	ALARM_REG = 1,
	ALARM_SHELF = 2,
	ALARM_OUT = 3
};

typedef struct ClientUpdateInfo
{
	int cID;
	int state;
	int motion; //1 punch, 2 raise_hand, 3 fall
	int alarm; // 1 reg 2 shelf 3 out
	std::vector<cv::Point> pos;
	time_t t;
};

typedef struct GoodsUpdateInfo
{
	int cID;
	int gID;
	int direction;
	int count;
	cv::Point pos;
	std::vector<GoodsScore> conf;
	time_t t;
};

typedef struct HandUpdateInfo
{
	cv::Point3f left, right;
	int leftState, rightState;

	cv::Point3f body_pos;
};

class GoodsClientMap
{
public:
	GoodsClientMap() {};
	~GoodsClientMap() {};

	bool update_client(const ClientUpdateInfo& cInfo)
	{
		auto pS = m_state.find(cInfo.cID);
		if (pS == m_state.end())
		{
			m_state[cInfo.cID] = cInfo.alarm; //cInfo.state;
			pS = m_state.find(cInfo.cID);
		}
		if (pS->second != ALARM_OUT)
			pS->second = cInfo.alarm;//cInfo.state;

		if (pS->second == ALARM_OUT)
		{
			m_histClient.insert(cInfo.cID);
			return false;
		}
		for (int k = 0; k < cInfo.pos.size(); k++)
		{
			if (cInfo.pos[k].x >= 0)
				m_clientPosition[cInfo.cID].push_back(std::make_pair(cInfo.t, cInfo.pos[k]));
		}
		return true;
	};

	int update_goods(const GoodsUpdateInfo& gInfo) //return goods id, or -1 if failed
	{
		auto pS = m_state.find(gInfo.cID);
		if (pS == m_state.end() || pS->second == ALARM_OUT)
		{
			return -1;
		}
		if (gInfo.direction == -2)
		{
			m_cart[gInfo.cID][gInfo.gID] = gInfo.count;
			return gInfo.gID;
		}
		else
		{
			if (gInfo.direction == 1)
			{
				m_cart[gInfo.cID][gInfo.gID] += gInfo.direction;
				if (m_cart[gInfo.cID][gInfo.gID] < 0)
					m_cart[gInfo.cID][gInfo.gID] = 0;
				m_goodsPosition[gInfo.cID].push_back(std::make_tuple(gInfo.t, gInfo.pos, gInfo.direction));
				return gInfo.gID;
				//m_conf[gInfo.cID][gInfo.gID].push_back(gInfo.conf);
			}
			else if (gInfo.direction == -1)
			{
				if (m_cart[gInfo.cID][gInfo.gID])
				{
					m_cart[gInfo.cID][gInfo.gID]--;
					return gInfo.gID;
					//m_conf[gInfo.cID][gInfo.gID].pop_back();
				}
				else
				{
					for (int i = 0; i < gInfo.conf.size(); i++)
					{
						if (m_cart[gInfo.cID][gInfo.conf[i].type])
						{
							m_cart[gInfo.cID][gInfo.conf[i].type]--;
							return gInfo.conf[i].type;
							//							break;
														//m_conf[gInfo.cID][gInfo.conf[i].type].pop_back();
						}
					}
				}
			}
		}
		return -1;
	};

	int update_hand(const std::vector<HandTrackerNode> & v);

	void clear()
	{
		m_state.clear();
		m_clientPosition.clear();
		m_cart.clear();
		m_goodsPosition.clear();
		m_histClient.clear();
	};

	inline std::map<int, int> getGoodsState(int id)
	{
		//printf("getGoodsState %d\n", id);
		std::map<int, int> res = m_cart[id];
		return res;
	};


	std::map<int, HandUpdateInfo> m_hand;
	std::map<int, int> m_state;
	std::map<int, std::vector<std::pair<time_t, cv::Point> > > m_clientPosition;
	std::map<int, std::map<int, int> > m_cart;
	std::map<int, std::map<int, std::list<std::vector<GoodsScore> > > > m_conf;
	std::map<int, std::vector<std::tuple<time_t, cv::Point, int> > > m_goodsPosition;
	std::set<int> m_histClient;
};


typedef struct CartStateAlarm
{
	int cId;//gId
	int alarm_type;
	std::map<int, int> goods_data;
};