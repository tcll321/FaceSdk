/**************************************
author: fwy
data:	2018.08.01
function:
face tracker use IOU
test SDK_imiLivenessDetection_S_V5

revise1: for eyeBlinkDetect, comment update liveness, if use depth liveness, should open it(2018080f by fwy)
revise2: for mouthMoveDetect, comment if (!trackers[maxT].still), if use depth liveness, should open it(2018080f by fwy)

****************************************/
#pragma once
#include <vector>
#include "imiCV.h"
//#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace imi;

#define MISS_FACE_FRAMES		3

struct trackFaces
{
	imi::imiRect bbox;
	int gender;
	int age;
	int id;
	int liveness;
	int missFrames;
	bool still;
	int showTimes;
	vector<int> vec_liveness;
	vector<int> vec_age;
	vector<int> vec_gender;
	float vec_mouthSts[2];
	vector<imi::imiPoint> traces;//鼻尖点组成
	trackFaces(imi::imiRect rec, int g, int a, int i)
	{
		bbox = rec;
		gender = g;
		age = a;
		id = i;
		liveness = 0;
		missFrames = 0;
		still = false;
		showTimes = 1;
		vec_mouthSts[0] = 0;
		vec_mouthSts[1] = 0;
	};
	trackFaces()
	{
		gender = -1;
		age = -1;
		id = -1;
		missFrames = 0;
		still = false;
		showTimes = 1;
		liveness = 0;
		vec_mouthSts[0] = 0;
		vec_mouthSts[1] = 0;
	};
};
vector<trackFaces> _g_imi_faceIOUtracker;//注意命名冲突问题
float IOUcalc(const imi::imiRect rec1, const imi::imiRect rec2, const std::string mode = "Union")
{
	float IOU = 0;
	float maxX = (rec1.x>rec2.x) ? rec1.x : rec2.x;
	float maxY = (rec1.y>rec2.y) ? rec1.y : rec2.y;
	float minX = ((rec1.x + rec1.width)<(rec2.x + rec2.width)) ? (rec1.x + rec1.width) : (rec2.x + rec2.width);
	float minY = ((rec1.y + rec1.height)<(rec2.y + rec2.height)) ? (rec1.y + rec1.height) : (rec2.y + rec2.height);
	//maxX1 and maxY1 reuse 
	maxX = ((minX - maxX + 1)>0) ? (minX - maxX + 1) : 0;
	maxY = ((minY - maxY + 1)>0) ? (minY - maxY + 1) : 0;
	//IOU reuse for the area of two bbox
	IOU = maxX * maxY; //cout << "IOU org= " << IOU <<" w h "<<maxX<<" "<<maxY<< endl;
	if (!mode.compare("Union"))
		IOU = IOU / (rec1.width*rec1.height + rec2.width*rec2.height - IOU);
	else if (!mode.compare("Min")){
		IOU = IOU / ((rec1.width*rec1.height<rec2.width*rec2.height) ? rec1.width*rec1.height : rec2.width*rec2.height);
	}
	return IOU;
}

//检查每个正在跟踪的对象，其最新连续未能检出次数是否超过MISS_FACE_FRAMES，若是，销毁该对象的跟踪
void updataTrackers(vector<trackFaces> &trackers)
{
	for (vector<trackFaces>::iterator it = trackers.begin(); it != trackers.end();)
	{
		/**********update gender(start)**************/
		if (it->vec_gender.size()>10)
		{
			it->vec_gender.erase(it->vec_gender.begin());
		}
		if (it->vec_gender.size() == 10)
		{
			int cnt_male = 0, cnt_female = 0,cnt_notsure=0;
			for (int i = 0; i<it->vec_gender.size(); i++)
			{
				if (it->vec_gender[i] == 1)cnt_male++;
				else if (it->vec_gender[i] == 0)cnt_female++;
			}
			if (cnt_male>cnt_female)it->gender = 1;
			else it->gender = 0;
		}
		///**********update gender(end)**************/

		/**********update age(start)**************/
		if (it->vec_age.size()>10)
		{
			it->vec_age.erase(it->vec_age.begin());
		}
		if (it->vec_age.size() == 10)
		{
			int sum_age = 0;
			for (int i = 0; i<it->vec_age.size(); i++)
			{
				sum_age += it->vec_age[i];
			}
			it->age = floor(sum_age/10);
		}
		///**********update age(end)**************/

		it->missFrames++;
		if (it->missFrames > MISS_FACE_FRAMES)
		{
			it = trackers.erase(it);
			//cout << "updata trackers, num: " << trackers.size() << endl;
		}
		else
		{
			it++;
		}
	}
}

//检查输入的人脸 是否能够与跟踪对象中的某个匹配上，如果匹配上，则继承该跟踪对象的属性
bool IOUtrackFace(FaceInfo &detFace, vector<trackFaces> &trackers, int &trackerID)
{
	detFace.gender = -1;
	detFace.age = -1;
	detFace.profile = -1;
	detFace.depth = -1.f;
	trackerID = -1;
	imi::imiRect cvBbox(detFace.bbox.x, detFace.bbox.y, detFace.bbox.width, detFace.bbox.height);
	bool find = false;
	if (trackers.size()>0)
	{
		int maxT = 0; float maxIOU = 0;
		for (int t = 0; t < trackers.size(); t++)
		{
			float DetIOU = IOUcalc(trackers[t].bbox, cvBbox, "Min");
			if (DetIOU>maxIOU){
				maxIOU = DetIOU; maxT = t;
			}
		}
		if (maxIOU>0.7)
		{
			trackers[maxT].bbox = cvBbox;//跟新tracker的位置
			trackers[maxT].missFrames = 0;

			/**********判断目标是否静止**********/
			if (!trackers[maxT].still)
			{
				trackers[maxT].traces.push_back(imi::imiPoint(detFace.landmark[2].x, detFace.landmark[2].y));

				float pos_threshold = detFace.bbox.width / 10.0;
				if (trackers[maxT].traces.size() > 5)
				{
					trackers[maxT].still = false;
					trackers[maxT].traces.erase(trackers[maxT].traces.begin());
				}
				if (trackers[maxT].traces.size() == 5)
				{
					float dist_head[4] = { 0 };
					trackers[maxT].still = true;
					for (int h_i = 0; h_i < 4; h_i++)
					{
						dist_head[h_i] = sqrt(pow(trackers[maxT].traces.at(4).x - trackers[maxT].traces.at(h_i).x, 2) + \
							pow(trackers[maxT].traces.at(4).y - trackers[maxT].traces.at(h_i).y, 2));
						if (dist_head[h_i] >pos_threshold)
						{
							trackers[maxT].still = false;
							break;
						}
					}
					if (trackers[maxT].still)
					{
						//cout<<"~~~~~~~~~~~~~~~~stand still now.~~~~~~~~~~~"<<endl;
					}
				}
				else
				{
					trackers[maxT].still = false;
				}
			}

			detFace.gender = trackers[maxT].gender;
			detFace.age = trackers[maxT].age;
			detFace.profile = trackers[maxT].id;
			detFace.depth = trackers[maxT].still;
			find = true;
			trackerID = maxT;
		}
	}
	return find;
}

