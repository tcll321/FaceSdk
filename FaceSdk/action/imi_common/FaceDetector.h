#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#ifdef USE_IMI_DET
#include <imiCV.h>
#endif

typedef struct FaceInfoNode
{
	cv::Rect r;
	std::vector<cv::Point2f> land;
	float yaw, roll, pitch;
	float score;
	int neighbour;
	FaceInfoNode():yaw(0),roll(0),pitch(0),score(0),neighbour(0) {
	}
};

typedef std::vector<FaceInfoNode> FaceInfoVec;
class FaceBaseDetector
{
public:
	static FaceBaseDetector* create(std::string type, std::string data_path = "");
	virtual int Init(std::string str = "")
	{
		return 0;
	}
	virtual int Detect(cv::Mat m, std::vector<FaceInfoNode> & res, bool bLand = true, bool bMax = false, bool bNormal = false)
	{
		std::cout << "no implementation\n";
		return -1;
	};
#ifdef USE_IMI_DET
	virtual int Detect(cv::Mat m, std::vector<imi::FaceInfo> & res, bool bLand = true, bool bMax = false, bool bNormal = false)
	{
		std::cout << " no impl\n";
		return -1;
	};
#endif
	virtual int Landmark(cv::Mat m, FaceInfoNode& face)
	{
		std::cout << "no implementation\n";
		return -1;
	};

	virtual std::vector<float> Feature(cv::Mat m, cv::Rect r, std::vector<cv::Point2f> points)
	{
		std::cout << " no implementation\n";
		return std::vector<float>();
	}
};