#pragma once

#pragma comment(lib, "libFaceDetect.lib")

#include "FaceDetector.h"
#include<libFaceDetect.h>
//#include<fa.h>
//#include<tbb\tbb.h>

class ImiDetector :public FaceBaseDetector
{
public:
	ImiDetector()
	{
		
	};
	int Init(std::string path)
	{
		std::string file_path = path.empty() ? "D:\\workspace\\IMI_dev\\SDKs\\Imi_FaceDetect_IR\\data" : path;
		//imi::Option option;
		//option.model_name = file_path + "\\FA_68_new.model";
		//IMI_FA_Initialization(option);
		imi::FaceDetectionInitialization();
		return 0;
	}
	~ImiDetector()
	{
		imi::FaceDetectionRelease();
	}
	virtual int Detect(cv::Mat im, std::vector<FaceInfoNode> & res, bool bLand = true, bool bMax = false, bool bNormal = false)
	{
		using namespace std;
		using namespace cv;
		using namespace imi;
		//using namespace tbb;

		if (im.empty())
		{
			std::cout << "image empty\n";
			return -1;
		}

		imiMat img(im.rows, im.cols, IMI_CV_8UC3, im.data);
		imiMat *pimg = &img;
		//CV_8UC3

		vector<imi::FaceInfo> results = imi::detectFace(&img);
		
		for (auto rr : results)
		{
			FaceInfoNode info;
			info.r = cv::Rect(rr.bbox.x, rr.bbox.y, rr.bbox.width, rr.bbox.height);
			for (int i = 0; i < 5; i++)
				info.land.push_back(cv::Point(rr.landmark[i].x, rr.landmark[i].y));
			res.push_back(info);
		}

		if (bMax)
		{
			int bigId = -1;
			int bigW = -1;
			for (int i = 0;i < res.size();i++)
			{
				if (res[i].r.width > bigW)
				{
					bigW = res[i].r.width;
					bigId = i;
				}
			}
			if (bigId >= 0)
			{
				FaceInfoNode info = res[bigId];
				res.clear();
				res.push_back(info);
			}
		}
#if 0
		if (bLand)
		{
			for (auto &rr : res)
			{
				imi::FD_Face detected_face;
				int y_off = 0;// res.bbox.height*0.25;
				int x_off = rr.r.height / 2;
				detected_face.pt[0].x = rr.r.x + rr.r.width / 2 - x_off;
				detected_face.pt[0].y = rr.r.y;

				detected_face.pt[1].x = rr.r.x + rr.r.width / 2 - x_off;
				detected_face.pt[1].y = rr.r.y + rr.r.height;

				detected_face.pt[2].x = rr.r.x + rr.r.width / 2 + x_off;
				detected_face.pt[2].y = rr.r.y + rr.r.height;

				detected_face.pt[3].x = rr.r.x + rr.r.width / 2 + x_off;
				detected_face.pt[3].y = rr.r.y;// +res.bbox.height;
				detected_face.ripAngle = 0;// -res.roll;
										   //if (res.profile < -30)
										   //	detected_face.type = imi::LEFT_HALF_FACE;
										   //else if (res.profile > 30)
										   //detected_face.type = imi::RIGHT_HALF_FACE;
										   //else
				detected_face.type = imi::FRONTAL_FACE;

				FaceShape68 dstShape;

				IMI_FaceAlignment68pts(&im, NULL, &detected_face, &dstShape);
				//FaceShape68 dstShape;
				//IMI_FaceAlignment68pts(&im, NULL, &detected_face, &dstShape);// 68 facial points detection.
				
				rr.land.clear();
				for (auto p : dstShape.points)
				{
					rr.land.push_back(cv::Point2f(p.x, p.y));
				}

			}
		}
#endif
		return 0;
	};

	virtual int Detect(cv::Mat im, std::vector<imi::FaceInfo> & res, bool bLand = true, bool bMax = false, bool bNormal = false)
	{
		using namespace std;
		using namespace cv;
		using namespace imi;
		//using namespace tbb;

		if (im.empty())
		{
			std::cout << "image empty\n";
			return -1;
		}

		imiMat img(im.rows, im.cols, IMI_CV_8UC3, im.data);
		imiMat *pimg = &img;
		//CV_8UC3

		vector<imi::FaceInfo> results = imi::detectFace(&img);

		//res.insert(res.end(), results.begin(), results.end());
		res = results;

		if (bMax)
		{
			int bigId = -1;
			int bigW = -1;
			for (int i = 0; i < res.size(); i++)
			{
				if (res[i].bbox.width > bigW)
				{
					bigW = res[i].bbox.width;
					bigId = i;
				}
			}
			if (bigId >= 0)
			{
				imi::FaceInfo info = res[bigId];
				res.clear();
				res.push_back(info);
			}
		}
#if 0
		if (bLand)
		{
			for (auto &rr : res)
			{
				imi::FD_Face detected_face;
				int y_off = 0;// res.bbox.height*0.25;
				int x_off = rr.r.height / 2;
				detected_face.pt[0].x = rr.r.x + rr.r.width / 2 - x_off;
				detected_face.pt[0].y = rr.r.y;

				detected_face.pt[1].x = rr.r.x + rr.r.width / 2 - x_off;
				detected_face.pt[1].y = rr.r.y + rr.r.height;

				detected_face.pt[2].x = rr.r.x + rr.r.width / 2 + x_off;
				detected_face.pt[2].y = rr.r.y + rr.r.height;

				detected_face.pt[3].x = rr.r.x + rr.r.width / 2 + x_off;
				detected_face.pt[3].y = rr.r.y;// +res.bbox.height;
				detected_face.ripAngle = 0;// -res.roll;
										   //if (res.profile < -30)
										   //	detected_face.type = imi::LEFT_HALF_FACE;
										   //else if (res.profile > 30)
										   //detected_face.type = imi::RIGHT_HALF_FACE;
										   //else
				detected_face.type = imi::FRONTAL_FACE;

				FaceShape68 dstShape;

				IMI_FaceAlignment68pts(&im, NULL, &detected_face, &dstShape);
				//FaceShape68 dstShape;
				//IMI_FaceAlignment68pts(&im, NULL, &detected_face, &dstShape);// 68 facial points detection.

				rr.land.clear();
				for (auto p : dstShape.points)
				{
					rr.land.push_back(cv::Point2f(p.x, p.y));
				}

			}
		}
#endif
		return 0;
	};

#if 0
	virtual int Landmark(cv::Mat im, FaceInfoVec& face)
	{
		for (auto rr : face)
		{
			imi::FD_Face detected_face;
			int y_off = 0;// res.bbox.height*0.25;
			int x_off = rr.r.height / 2;
			detected_face.pt[0].x = rr.r.x + rr.r.width / 2 - x_off;
			detected_face.pt[0].y = rr.r.y;

			detected_face.pt[1].x = rr.r.x + rr.r.width / 2 - x_off;
			detected_face.pt[1].y = rr.r.y + rr.r.height;

			detected_face.pt[2].x = rr.r.x + rr.r.width / 2 + x_off;
			detected_face.pt[2].y = rr.r.y + rr.r.height;

			detected_face.pt[3].x = rr.r.x + rr.r.width / 2 + x_off;
			detected_face.pt[3].y = rr.r.y;// +res.bbox.height;
			detected_face.ripAngle = 0;// -res.roll;
									   //if (res.profile < -30)
									   //	detected_face.type = imi::LEFT_HALF_FACE;
									   //else if (res.profile > 30)
									   //detected_face.type = imi::RIGHT_HALF_FACE;
									   //else
			detected_face.type = imi::FRONTAL_FACE;

			imi::FaceShape68 dstShape;

			IMI_FaceAlignment68pts(&im, NULL, &detected_face, &dstShape);
			//FaceShape68 dstShape;
			//IMI_FaceAlignment68pts(&im, NULL, &detected_face, &dstShape);// 68 facial points detection.

			rr.land.clear();
			for (auto & p : dstShape.points)
			{
				rr.land.push_back(cv::Point2f(p.x, p.y));
			}

		}
		return 0;
	};
#endif
}; 
