#pragma once

#include <opencv2/opencv.hpp>
#include <ImiSkeleton.h>
#pragma comment(lib, "strmiids.lib")
class SkeletonShow
{
public:
	static void drawSkeleton(cv::Mat& m, ImiSkeletonFrame* pSkeleton)
	{
		if (m.empty())
			return;

		for (int i = 0; i < MAX_TRACKED_PEOPLE_NUM; ++i)
		{
			const ImiSkeletonData* pSkeletonData = &pSkeleton->skeletonData[i];

			if (IMI_SKELETON_TRACKED == pSkeletonData->trackingState)
			{

				int width = m.cols;
				int height = m.rows;

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_HEAD,
					IMI_SKELETON_POSITION_SHOULDER_CENTER, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_SHOULDER_CENTER,
					IMI_SKELETON_POSITION_SHOULDER_LEFT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_SHOULDER_CENTER,
					IMI_SKELETON_POSITION_SHOULDER_RIGHT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_SHOULDER_CENTER,
					IMI_SKELETON_POSITION_SPINE, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_SPINE,
					IMI_SKELETON_POSITION_HIP_CENTER, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_HIP_CENTER,
					IMI_SKELETON_POSITION_HIP_LEFT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_HIP_CENTER,
					IMI_SKELETON_POSITION_HIP_RIGHT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_SHOULDER_LEFT,
					IMI_SKELETON_POSITION_ELBOW_LEFT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_ELBOW_LEFT,
					IMI_SKELETON_POSITION_WRIST_LEFT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_WRIST_LEFT,
					IMI_SKELETON_POSITION_HAND_LEFT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_SHOULDER_RIGHT,
					IMI_SKELETON_POSITION_ELBOW_RIGHT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_ELBOW_RIGHT,
					IMI_SKELETON_POSITION_WRIST_RIGHT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_WRIST_RIGHT,
					IMI_SKELETON_POSITION_HAND_RIGHT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_HIP_LEFT,
					IMI_SKELETON_POSITION_KNEE_LEFT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_KNEE_LEFT,
					IMI_SKELETON_POSITION_ANKLE_LEFT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_ANKLE_LEFT,
					IMI_SKELETON_POSITION_FOOT_LEFT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_HIP_RIGHT,
					IMI_SKELETON_POSITION_KNEE_RIGHT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_KNEE_RIGHT,
					IMI_SKELETON_POSITION_ANKLE_RIGHT, width, height);

				drawBone(m, pSkeletonData, IMI_SKELETON_POSITION_ANKLE_RIGHT,
					IMI_SKELETON_POSITION_FOOT_RIGHT, width, height);
			}
		}
	}
	static void convert(ImiVector4I& joint, ImiVector4 pos, int h, int w)
	{
		if (pos.z == 0)
			return;
		joint.x = w / 2 + pos.x*w / 320 / pos.z * 300;
		joint.y = h / 2 + pos.y*w / 320 / pos.z * 300;
	};

	static void drawBone(cv::Mat & m, const ImiSkeletonData* pSkeletonData, int first, int second, int width, int height, cv::Scalar s = cv::Scalar(0,255,0))
	{
		ImiSkeletonPositionTrackingState state0 = pSkeletonData->skeletonPositionTrackingState[first];
		ImiSkeletonPositionTrackingState state1 = pSkeletonData->skeletonPositionTrackingState[second];

		if ((IMI_SKELETON_POSITION_NOT_TRACKED == state0) || (IMI_SKELETON_POSITION_NOT_TRACKED == state1)) {
			return;
		}

		// Both are inferred
		if ((IMI_SKELETON_POSITION_INFERRED == state0) && (IMI_SKELETON_POSITION_INFERRED == state1)) {
			return;
		}

		ImiVector4I joint1;
		//convert(joint1, pSkeletonData->skeletonPositions[first], height, width);
		imiConvertCoordinateWorldToDepth(&joint1, &pSkeletonData->skeletonPositions[first], height, width);

		ImiVector4I joint2;
		//convert(joint2, pSkeletonData->skeletonPositions[second], height, width);
		imiConvertCoordinateWorldToDepth(&joint2, &pSkeletonData->skeletonPositions[second], height, width);

		cv::line(m, cv::Point(joint1.x, joint1.y), cv::Point(joint2.x, joint2.y), s, 3);
		cv::circle(m, cv::Point(joint1.x, joint1.y), 3, cv::Scalar(0, 0, 255), -1);		
		cv::circle(m, cv::Point(joint2.x, joint2.y), 3, cv::Scalar(0, 0, 255), -1);
	}
};