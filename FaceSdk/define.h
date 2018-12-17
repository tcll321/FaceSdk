#ifndef __DEFINE_H__
#define __DEFINE_H__

enum SexType
{
	Female=0,
	Male
};

typedef struct _FaceInfoStruct
{
	unsigned int age; //年龄
	unsigned int sex; //性别 0 女 1 男
}_FaceInfo;

typedef struct _TrajectoryStruct
{
	unsigned int total ;//总人数
	unsigned int fall;	//跌倒
	unsigned int punch;	//出拳
	unsigned int raise;	//举手
	unsigned int nTracker; //当前图片中的人数
	int	id;			//id
	int x;			//x坐标
	int y;			//y坐标
	int motion;		//运动姿态 punch = 1, raise_hand = 2, fall = 3
}_TrajectoryInfo;

#endif