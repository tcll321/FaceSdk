#pragma once

#include "FaceDetector.h"
#include <opencv2/opencv.hpp>
#include "../headDetector/ip_funcs.h"

#include "MaskProcess.h"

class DepthDetector : public FaceBaseDetector
{
public:
	DepthDetector() :FaceBaseDetector()
	{

	}
	void Init(std::string path = "")
	{
		FILE * fp;
		std::string str_d = path.empty() ? "D:/workspace/IMI_dev/Liveness/data/depth_detector.dat" : path;
		fopen_s(&fp, str_d.c_str(), "rb");
		if (fp)
		{
			fread_s(&gHDRF, sizeof(HDRF), 1, sizeof(HDRF), fp);
			fclose(fp);
		}
		else
		{
			printf("Initializing failed 1!\n");
		}
	}

	virtual int Detect(cv::Mat m, std::vector<FaceInfoNode> & res, bool bLand = true, bool bMax = false, bool bNormal = false)
	{
		//input 16U
		cv::Mat m16u, m16u_resize;
		
		if (m.type() & CV_MAT_DEPTH_MASK != CV_16U)
		{
			m.convertTo(m16u, CV_16U);
		}
		else
			m16u = m;

		cv::resize(m16u, m16u_resize, cv::Size(640, 480));
		
		unsigned char M[60][80];
		Nui_GotDepthAlert_HeadDetector((ushort *)m16u_resize.data, M);
		
		cv::Mat head_depth(60, 80, CV_8UC1, &M);
		cv::resize(head_depth, head_depth, cv::Size(), 4, 4, 1);
		cv::imshow("head_depth", head_depth);

		return 0;
	};
	virtual int Landmark(cv::Mat m, FaceInfoNode& face)
	{
		std::cout << "no implementation\n";
		return -1;
	};


	void Nui_GotDepthAlert_HeadDetector(const unsigned short * pDepthBGR640, unsigned char  pClass[60][80], int MIN_DISTANCE = 200, int MAX_DISTANCE = 1000)
	{
		//	unsigned char  pClass[60][80];
		memset(pClass, 0, 60 * 80);
		const int Res = 640;
		for (int j = 0; j < 60; j++)
		{
			for (int i = 0; i < 80; i++)
			{
				int dx = (pDepthBGR640[(j << 3) * Res + (i << 3)]);//	int dx = (pDepthBGR320[(j << 3) * 640 + (i << 3)] >> 3);
																   //cout << "int dx" << dx << endl;
				if (dx > MIN_DISTANCE && dx <= MAX_DISTANCE)// [0m~4m], kinect shot range.
				{
					int values = 0;
					float dxinv = 1.0f / float(dx);
					for (int tid = 0; tid < 3; tid++)
					{
						int value = 0;
						for (int nid = 0, level = 0; level < 14; level++)// orig: level<14
						{
							HDRF::_NODE * pNode = &(gHDRF.node[tid * 16383 + nid]);

							//TRACE("tid = %d, level = % 2d, val = {% 3d, % 3d}\n", tid, level, pNode->v1, pNode->v2);

							int p1x, p1y, p2x, p2y;
							p1x = int((pNode->p1u * 256.0f - 32512.0f) * dxinv) + i;
							p1y = int((pNode->p1v * 256.0f - 32512.0f) * dxinv) + j;
							p2x = int((pNode->p2u * 256.0f - 32512.0f) * dxinv) + i;
							p2y = int((pNode->p2v * 256.0f - 32512.0f) * dxinv) + j;

							int thres = (pNode->thres & 0xFFFF3FFF) - 0x2000;
							int mark = pNode->thres & 0x4000;

							int val1 = 6000, val2 = 6000;
							if (p1y >= 0 && p1y < 60 && p1x >= 0 && p1x < 80)
							{
								val1 = (pDepthBGR640[(p1y << 3) * Res + (p1x << 3)]); //val1 = (pDepthBGR640[(p1y << 3) * Res + (p1x << 3)] >> 3);
								val1 = (val1 <= 4000) ? val1 : 6000;
							}
							if (mark == 0)
							{
								val2 = dx;
							}
							else if (p2y >= 0 && p2y < 60 && p2x >= 0 && p2x < 80)
							{
								val2 = (pDepthBGR640[(p2y << 3) * Res + (p2x << 3)]);//			val2 = (pDepthBGR640[(p2y << 3) * Res + (p2x << 3)] >> 3);
								val2 = (val2 <= 4000) ? val2 : 6000;
							}

							if (val1 - val2 < thres)
							{
								nid = nid * 2 + 1;
								value = pNode->v1;
							}
							else
							{
								nid = nid * 2 + 2;
								value = pNode->v2;
							}
						}
						values += value;
					} // end of tid

					pClass[j][i] = values / 3;
				}
				else
				{
					pClass[j][i] = 0;
				}
			}
		}
		return;
	}

	HDRF gHDRF;
};