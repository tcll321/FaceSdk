#ifndef _DETECTION_DATA_H
#define _DETECTION_DATA_H

#include <opencv2/opencv.hpp>
#include "../common/Config/ColorCameraConfig.h"
#include "JSON/cJSON.h"
#include "putText.h"
#include <ShowUtils.h>
#include "direction.h"
#include "BasicStructs.h"
#if 0
class DetectionData
{
public:
	int channel;
	std::vector<int> type;
	std::vector<float> confidence;
	std::vector<cv::Rect> r;
	double stamp;
	//debug
	int bTracked = 0;
};
#endif


typedef struct GoodsRawInfo
{
	int shelfID = 0;
	int camera = 0;
	time_t stamp;
	cv::Point3f body_pos;
	cv::Point3f hand_pos;
	int goodsID = -1;
	//float goodsProb = 0;
	std::vector<GoodsScore> goodsProb;
	int direction = 0;
}GoodsRawInfo;


static cJSON* to_json(const GoodsRawInfo& info)
{
	cJSON* root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "sID", info.shelfID);
	cJSON_AddNumberToObject(root, "gID", info.goodsID);
	cJSON_AddNumberToObject(root, "cID", info.camera);
	cJSON_AddNumberToObject(root, "bodyX", info.body_pos.x);
	cJSON_AddNumberToObject(root, "bodyY", info.body_pos.y);
	cJSON_AddNumberToObject(root, "bodyZ", info.body_pos.z);
	cJSON_AddNumberToObject(root, "handX", info.hand_pos.x);
	cJSON_AddNumberToObject(root, "handY", info.hand_pos.y);
	cJSON_AddNumberToObject(root, "handZ", info.hand_pos.z);
	cJSON* pProb = cJSON_AddArrayToObject(root, "prob");
	for (int i = 0; i < info.goodsProb.size(); i++)
	{
		cJSON* pS = cJSON_CreateObject();
		cJSON_AddNumberToObject(pS, "t", info.goodsProb[i].type);
		cJSON_AddNumberToObject(pS, "s", info.goodsProb[i].score);
		cJSON_AddItemToArray(pProb, pS);
	}
	//cJSON_AddNumberToObject(root, "prob", info.goodsProb);
	cJSON_AddNumberToObject(root, "d", info.direction);
	//cJSON_AddNumberToObject(root, )
	return root;
}

static GoodsRawInfo from_json(cJSON* pJson) {
	GoodsRawInfo info;
	cJSON* pShelf = cJSON_GetObjectItem(pJson, "sID");
	info.shelfID = pShelf->valueint;// atoi(cJSON_GetStringValue(pCamera));
	cJSON* pType = cJSON_GetObjectItem(pJson, "gID");
	info.goodsID = pType->valueint;//atoi(cJSON_GetStringValue(pType));
	cJSON* pCam = cJSON_GetObjectItem(pJson, "cID");
	info.camera = pCam->valueint;

	cJSON* pB = cJSON_GetObjectItem(pJson, "bodyX");
	info.body_pos.x = pB->valuedouble;//atof(cJSON_GetStringValue(pConf));	
	pB = cJSON_GetObjectItem(pJson, "bodyY");
	info.body_pos.y = pB->valuedouble;
	pB = cJSON_GetObjectItem(pJson, "bodyZ");
	info.body_pos.z = pB->valuedouble;

	cJSON* pH = cJSON_GetObjectItem(pJson, "handX");
	info.body_pos.x = pH->valuedouble;//atof(cJSON_GetStringValue(pConf));	
	pH = cJSON_GetObjectItem(pJson, "handY");
	info.body_pos.y = pH->valuedouble;
	pH = cJSON_GetObjectItem(pJson, "handZ");
	info.body_pos.z = pH->valuedouble;

	cJSON* pProb = cJSON_GetObjectItem(pJson, "prob");
	if (pProb)
	{
		int len = cJSON_GetArraySize(pProb);
		for (int i = 0; i < len; i++)
		{
			cJSON* pElem = cJSON_GetArrayItem(pProb, i);
			GoodsScore score;
			score.type = cJSON_GetObjectItem(pElem, "t")->valueint;
			score.score = cJSON_GetObjectItem(pElem, "s")->valuedouble;

			info.goodsProb.push_back(score);
		}
	}
	//info.goodsProb = pP->valuedouble;

	cJSON* pDirection = cJSON_GetObjectItem(pJson, "d");
	info.direction = pDirection->valueint;//atoi(cJSON_GetStringValue(pDirection));
	return info;
}
#if 0
class GoodsRawInfo :public ObjectMovingAlarm
{
public:

};
#endif
//typedef struct _GoodsRawInfo
//{
//	int camera;
//	int type;
//	float conf;
//	int direction;
//	//
//	//double stamp;
//} GoodsRawInfo;

inline std::map<int, std::string> load_label(const char* file)
{
	std::ifstream ifs(file);
	std::map<int, std::string> names;
	if (!ifs.is_open())
		return names;
	int count = 0;
	while (!ifs.eof())
	{
		ifs >> names[count++];
	}
	printf("loaded labels :\n");
	for (auto & item : names)
	{
		std::cout << "\t" << item.first << "----" << item.second << std::endl;
	}

	return names;
}
static std::map<int, std::string> g_label = load_label("./data/obj.names");
inline void draw_lines(cv::Mat& m, const ColorCameraConfig& data, int state = 0, int outDirection = 1)
{
	cv::Scalar c = cv::Scalar(0, 255, 255);
	if (state == 1)
		c = cv::Scalar(0, 0, 255);
	else if (state == -1)
		c = cv::Scalar(0, 255, 0);
	cv::rectangle(m, data.cutRect, c, 5);
	cv::Point st, ed;
	if (outDirection == CC_OUT_LEFT || outDirection == CC_OUT_UP)
	{
		st = data.cutRect.tl();
	}
	else
	{
		st = data.cutRect.br();
	}
	if (outDirection == CC_OUT_DOWN || outDirection == CC_OUT_LEFT)
	{
		ed = cv::Point(data.cutRect.x, data.cutRect.y + data.cutRect.height);
	}
	else {
		ed = cv::Point(data.cutRect.x + data.cutRect.width, data.cutRect.y);
	}
	cv::line(m, st, ed, cv::Scalar(0, 255, 0), 2);
	int offX = 0, offY = 0;
	if (outDirection == CC_OUT_RIGHT)
		offX = data.cutRect.width*data.center_line_ratio;
	else if (outDirection == CC_OUT_LEFT)
		offX = -data.cutRect.width*data.center_line_ratio;
	else if (outDirection == CC_OUT_UP)
		offY = -data.cutRect.height*data.center_line_ratio;
	else
		offY = data.cutRect.height*data.center_line_ratio;
	cv::line(m, st - cv::Point(offX, offY), ed - cv::Point(offX, offY), cv::Scalar(0, 0, 255), 2);
}

inline void _draw_detection(cv::Mat& m, cv::Rect r, int type)
{
	std::string szLabel = g_label[type];
	cv::Scalar c = ShowUtils::level_color((type * 76 + 51) % 31 / 30.0);

	cv::rectangle(m,r, c, 2);
	//cv::putText(m, szLabel, data.r[i].tl(), CV_FONT_HERSHEY_COMPLEX,)
	putTextZH(m, szLabel.c_str(), r.tl(), cv::Scalar(255, 0, 255), 22, "Î¢ÈíÑÅºÚ");
}

inline void draw_detection(cv::Mat& m, const shop::Detection& data)
{
	for (int i = 0; i < data.objects.size(); i++)
	{
		_draw_detection(m, data.objects[i].r, data.objects[i].id);
	}
}

class DetectionBatchData
{
public:
	std::vector<shop::FrameEx> vr;
	double stamp;
};


#endif // !_DETECTION_DATA_H