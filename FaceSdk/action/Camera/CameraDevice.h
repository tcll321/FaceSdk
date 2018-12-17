#pragma once
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "../common/Buffer/CycleBuffer.h"
#include "../common/BasicStructs.h"
#include<memory>
namespace autoshop
{
	enum CAMERA_TYPE
	{
		UVC = 1,
		A100 = 2,
		A100M = 3,
		RTSP = 4
	};
	enum CHANNEL_TYPE
	{
		COLOR = 1,
		DEPTH = 2,
		IR = 4,
		GRAVITY = 8,
		SKELETON = 16
	};
	enum STREAM_STATE
	{
		STOP = 0,
		RUN = 1,
		PAUSE = 2,
		UNAVLIABLE = -1
	};
	enum FETCH_TYPE
	{
		CYCLE = 1,
		QUEUE = 2,
		PULL = 3,
		WAIT = 8,
		OVERLAP = 16
	};


	struct CameraInfo
	{
		int id;
		std::string name;

		int type;
		std::string path;

		float fps;
		float delay;
		int max_frames;

		int channel_flag;
		int fetch_flag;

		std::map<int, cv::Size> channel_size;
	};

	typedef std::vector<shop::Frame> StreamData;

	class CameraDeviceInterface
	{
	public:
		static std::shared_ptr<CameraDeviceInterface> CreateCameraDevice(const CameraInfo& info);

		virtual bool open() = 0;
		virtual void close() = 0;
		
		//virtual bool restart();
		//virtual void setFps(float fps);
		//virtual void setDelay(float delay);
		//virtual bool isOpen();
		//virtual cv::Size getSize(int channel_type);
		//virtual bool setSize(int channel_type, cv::Size sz);

		virtual bool fetch(StreamData& data) = 0;
		virtual bool fetch_near(StreamData & data, double stamp) = 0;
	};
}