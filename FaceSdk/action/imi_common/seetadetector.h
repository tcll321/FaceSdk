#pragma once

#include "FaceDetector.h"

#include <face_identification.h>
#include <face_detection.h>
#include <face_alignment.h>
#include <opencv2/opencv.hpp>
#include <string>

typedef struct SeetaFaceNode:public FaceInfoNode
{
	SeetaFaceNode() :FaceInfoNode()
	{

	}
	SeetaFaceNode(seeta::FaceInfo info)
	{
		r = cv::Rect(info.bbox.x, info.bbox.y, info.bbox.width, info.bbox.height);
		roll = info.roll;
		pitch = info.pitch;
		yaw = info.yaw;
		score = info.score;
	}

	seeta::FaceInfo to_faceinfo()
	{
		seeta::FaceInfo info;
		info.bbox.x = r.x;
		info.bbox.y = r.y;
		info.bbox.width = r.width;
		info.bbox.height = r.height;
		info.roll = roll;
		info.pitch = pitch;
		info.yaw = yaw;
		info.score = score;
		return info;
	}

	SeetaFaceNode(const FaceInfoNode& n)
	{
		this->r = n.r;
		this->land = n.land;
		this->roll = n.roll;
		this->pitch = n.pitch;
		this->yaw = n.yaw;
		this->score = n.score;
		this->neighbour = n.neighbour;
	}

} SeetaFaceNode;

class SeetaDetector:public FaceBaseDetector
{
public:
	SeetaDetector() {};
	~SeetaDetector() {};

	int Init(std::string path = "")
	{
		if (path.empty())
			path = "D:/workspace/SeetaFaceEngine/model/";
		std::string szFD = path + "/" + "seeta_fd_frontal_v1.0.bin";
		std::string szFA = path + "/" + "seeta_fa_v1.1.bin";
		std::string szID = path + "/" + "seeta_fr_v1.0.bin";
		if (1)
		{
			FILE* p = fopen(szFD.c_str(), "r");
			if (p)
			{
				fclose(p);
			}
			else
			{
				szFD = "./seeta_fd_frontal_v1.0.bin";
				szFA = "./seeta_fa_v1.1.bin";
				p = fopen(szFD.c_str(), "r");
				if (p)
				{
					fclose(p);
				}
				else
				{
					return -1;
				}
			}
		}
		m_pFD = std::auto_ptr<seeta::FaceDetection>(new seeta::FaceDetection(szFD.c_str()));
		m_pFA = std::auto_ptr<seeta::FaceAlignment>(new seeta::FaceAlignment(szFA.c_str()));
		try {
			m_pID = std::auto_ptr<seeta::FaceIdentification>(new seeta::FaceIdentification(szID.c_str()));
		}
		catch (...)
		{
			return -1;
		}
		return 0;
	};
	
	int Detect(cv::Mat m, std::vector<FaceInfoNode>& vec, bool bLand = true, bool bMax = false, bool bNormal = false)
	{
		if (m.channels() == 3)
		{
			cv::Mat gray;
			cv::cvtColor(m, gray, CV_BGR2GRAY);

			m = gray;
		}
		if (bNormal)
		{

			cv::equalizeHist(m, m);
			//		cv::imshow("eq", m);
		}
		seeta::ImageData image;
		image.data = m.data;
		image.height = m.rows;
		image.width = m.cols;
		image.num_channels = m.channels();
		std::vector<seeta::FaceInfo> a = m_pFD->Detect(image);
		vec.clear();
		if (!bMax)
		{
			for (auto item : a)
			{
				SeetaFaceNode node(item);
				if (bLand)
				{
					seeta::FacialLandmark points[5];
					bool bGet = m_pFA->PointDetectLandmarks(image, item, points);
					if (bGet)
					{
						for (int i = 0; i < 5; i++)
						{
							node.land.push_back(cv::Point2f(points[i].x, points[i].y));
						}
					}
				}

				vec.push_back(node);
			}
		}
		else
		{
			int max_w = -1;
			int max_id = -1;
			for (int i = 0; i < a.size(); i++)
			{
				if (a[i].bbox.width > max_w)
				{
					max_w = a[i].bbox.width;
					max_id = i;
				}
			}
			if (max_id >= 0)
			{
				SeetaFaceNode node(a[max_id]);
				if (bLand)
				{
					seeta::FacialLandmark points[5];
					bool bGet = m_pFA->PointDetectLandmarks(image, a[max_id], points);
					if (bGet)
					{
						for (int i = 0; i < 5; i++)
						{
							node.land.push_back(cv::Point2f(points[i].x, points[i].y));
						}
					}
				}
				vec.push_back(node);
			}
		}

		return 0;
	};
	int Landmark(cv::Mat m, FaceInfoNode& node)
	{
		if (m.channels() == 3)
		{
			cv::Mat gray;
			cv::cvtColor(m, gray, CV_BGR2GRAY);

			m = gray;
		}

		seeta::ImageData image;
		image.data = m.data;
		image.height = m.rows;
		image.width = m.cols;
		image.num_channels = m.channels();
		SeetaFaceNode s = node;
		seeta::FacialLandmark points[5];
		bool bGet = m_pFA->PointDetectLandmarks(image, s.to_faceinfo(), points);
		if (bGet)
		{
			for (int i = 0; i < 5; i++)
			{
				node.land.push_back(cv::Point2f(points[i].x, points[i].y));
			}
			return 0;
		}
		return 1;
	};

	std::vector<float> Feature(cv::Mat m, cv::Rect r, std::vector<cv::Point2f> land)
	{
		if (m.empty() || !m_pID.get())
			return std::vector<float>();

		std::vector<float> vFeat;

		seeta::ImageData image;
		image.data = m.data;
		image.width = m.cols;
		image.height = m.rows;
		image.num_channels = 3;

		if (!land.empty())
		{
			seeta::FacialLandmark pp[5];
			for (int i = 0; i < 5; i++)
			{
				pp[i].x = land[i].x;
				pp[i].y = land[i].y;
			}
			FaceFeatures ff = new float[m_pID->feature_size()];
			m_pID->ExtractFeatureWithCrop(image, pp, ff);
			vFeat = std::vector<float>(ff, ff + m_pID->feature_size());
			delete ff;
		}
		else
		{
#if 0
			FaceFeatures ff = new float[m_pID->feature_size()];
			m_pID->ExtractFeature(image, ff);
			vFeat = std::vector<float>(ff, ff + m_pID->feature_size());
			delete ff;
#endif
		}
		return vFeat;
	}
protected:
	std::auto_ptr<seeta::FaceDetection> m_pFD;
	std::auto_ptr<seeta::FaceAlignment> m_pFA;
	std::auto_ptr<seeta::FaceIdentification> m_pID;
};