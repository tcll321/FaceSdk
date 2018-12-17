#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include "ReadIni.h"
#include <ImiSkeleton.h>

namespace shop
{
	typedef struct FrameMeta
	{
		int camera_id;
		int64 t;
	} FrameMeta;


	typedef struct Frame
	{
		FrameMeta meta;
		cv::Mat m;
		Frame() {};

		Frame(cv::Mat m, int camera_id)
		{
			meta.t = cv::getTickCount();
			meta.camera_id = camera_id;
			this->m = m;
		}
	} Frame;

	typedef struct Object
	{
		cv::Rect r;
		int id;
		float confidence;
	} Object;

	typedef struct Detection
	{
		std::vector<Object> objects;
	} Detection;

	typedef struct FrameEx : Frame
	{
		Detection det;
	} FrameEx;


	typedef struct TrackRes
	{
		std::vector<int> object_id;
		std::vector<float> confidence;
		int appear_type;
		int64 t_start, t_end;
	} TrackRes;

	typedef struct CameraSetting
	{
		int channel = 0;
		int id = -1;
		int rightOut = 1;
		int tX = 0, bX = 0;
		int debug = 1;
		int detect_width = 0;
		int alarm_width = 0;
		int interval = 50; //20 fps
		cv::Rect ROI;
		cv::Size size = cv::Size(640, 480);

		CameraSetting() {}
		CameraSetting(int cam, std::string cfg)
		{
			char buf[16];
			sprintf(buf, "CAMERA%d", cam);
			id = cam;
			ReadIni::load_param<int>(buf, "channel", channel, cfg.c_str());

			std::string szSize;
			ReadIni::load_string(buf, "size", "", cfg.c_str(), szSize);
			sscanf(szSize.c_str(), "%d,%d", &size.width, &size.height);

			ReadIni::load_param<int>(buf, "TX", tX, cfg.c_str());
			ReadIni::load_param<int>(buf, "BX", bX, cfg.c_str());

			ReadIni::load_param<int>(buf, "max_width", detect_width, cfg.c_str());
			ReadIni::load_param<int>(buf, "mid_width", alarm_width, cfg.c_str());
			ReadIni::load_param<int>(buf, "direction", rightOut, cfg.c_str());

			ReadIni::load_param<int>(buf, "interval", interval, cfg.c_str());

			if (rightOut == 1)//left->right
			{
				int minX = (std::max)(0, (std::min)(tX, bX));
				if (detect_width == 0)
					detect_width = size.width - minX;
				detect_width = (std::min)(detect_width, size.width - minX);
				ROI = cv::Rect(minX, 0, detect_width, size.height);
			}
			else
			{
				int maxX = (std::min)(size.width, (std::max)(tX, bX));
				if (detect_width == 0)
					detect_width = maxX;
				detect_width = (std::min)(detect_width, maxX);
				ROI = cv::Rect(maxX - detect_width, 0, detect_width, size.height);
			}

			if (alarm_width == 0)
				alarm_width = detect_width / 2;
			else if (alarm_width > detect_width)
				alarm_width = detect_width;

			ReadIni::load_param<int>(buf, "debug", debug, cfg.c_str());
		}
	} CameraSetting;

	typedef struct GoodsSetting
	{
		int type_count = 0;
		std::vector<std::string> name;
		GoodsSetting(std::string cfg)
		{
			std::string szNameFile;
			ReadIni::load_string("GOODS", "name_list", 0, cfg.c_str(), szNameFile);
			if (szNameFile.empty())
				szNameFile = "obj.names";

			std::ifstream ifs(szNameFile);
			if (ifs.is_open())
			{
				while (!ifs.eof())
				{
					std::string szLine;
					std::getline(ifs, szLine);
					if (szLine.empty())
						continue;
					type_count++;
					name.push_back(szLine);
				}
			}
		}
	}GoodsSetting;

	typedef struct TrackerSetting
	{
//		CameraSetting camera;
		
		int back_trace_count = 5;
		int lost_thresh = 5;
		float add_thresh = 0;// 0.01;//0.5;
		float IOU_thresh = 0.5;
		float IOU_merge_thresh = 0.5;
		float nms_thresh = 0.8;
		float area_diff_thresh = 0.25;
		float track_thresh = 100.0;//2.0;

		TrackerSetting() {};
		TrackerSetting(std::string cfg)
		{
//			camera = CameraSetting(cam, cfg);
			ReadIni::load_param<int>("TRACKER", "back_trace", back_trace_count, cfg.c_str());
			ReadIni::load_param<float>("TRACKER", "iou_thresh", IOU_thresh, cfg.c_str());
			ReadIni::load_param<float>("TRACKER", "iou_merge_thresh", IOU_merge_thresh, cfg.c_str());
			ReadIni::load_param<float>("TRACKER", "nms_thresh", nms_thresh, cfg.c_str());
			ReadIni::load_param<int>("TRACKER", "lost_thresh", lost_thresh, cfg.c_str());
			ReadIni::load_param<float>("TRACKER", "area_diff_thresh", area_diff_thresh, cfg.c_str());
			ReadIni::load_param<float>("TRACKER", "add_thresh", add_thresh, cfg.c_str());
			ReadIni::load_param<float>("TRACKER", "track_thresh", track_thresh, cfg.c_str());
		}
	} TrackerSetting;

	typedef struct CartSetting
	{
		GoodsSetting name;


	} CartSetting;

	typedef struct ObjInfo
	{
		int id;
		int64 t_start, t_end;
		int count;
		int lost;
		int s_start, s_end;

		cv::Rect start_pos;
		cv::Rect last_pos;

		std::vector<float> conf;
		std::vector<float> conf_count;
	};

	typedef struct ObjEvent
	{
		shop::FrameMeta meta;
		int type = -1;
		int state = 0;
		float conf = 0;
		ObjInfo obj;
	};
}


typedef struct HumanTrackerFrameData
{
	std::vector<cv::Mat> vColor;
	std::vector<cv::Mat> vDepth;
	std::vector<ImiSkeletonFrame> vSkeleton;
};

typedef struct GoodsScore
{
	int type;
	float score;
};

typedef void(*trackerOnStart)(void* hdl);
typedef void(*trackerOnStop)(void* hdl);
typedef void(*trackerImageFeedFunc)(void* hdl, shop::Frame);
typedef void(*trackerDetectionFeedFunc)(void* hdl, std::vector<shop::Detection>);