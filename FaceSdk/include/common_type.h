#pragma once

struct shop_rect_t {
	int x, y, w, h;
};

struct shop_point2f_t {
	float x, y;
};

struct shop_point_t {
	int x, y;
};

struct shop_point3f_t {
	float x, y, z;
};

struct shop_image_t
{
	int w, h, c, esz, step;
	unsigned char* data;
};

struct object_det_t
{
	int type;
	float prob;
	//	float* vProb;
}; 

struct tracker_t
{
	shop_point2f_t pos; //当前位置
	int state;//当前帧是否跟踪了
	int alarm;
	int id;//人的id

	shop_point2f_t* pos_data;  //历史位置，刚进店时的轨迹信息
	int pos_count; //历史位置数量

	int rect_count;
	shop_rect_t* rect_data;

	int motion; //运动姿态 punch = 1, raise_hand = 2, fall = 3
};

struct tracker_stat_t
{
	int total;
	int punch;
	int fall;
	int raise;
};
struct score_t
{
	int type;
	int score;
};

struct cart_tracker_result_t
{
	shop_point3f_t body;
	shop_point3f_t hand;
	int direction;
	int type;
	score_t* score_data;
	int score_count;
	//float score;
	int shelfID;
};

struct goods_single_t
{
	int type;
	int count;
};

struct cart_state_t
{
	int id;//gId
	int goods_count;  //商品大类数量
	goods_single_t* goods_data;
};

struct face_info_t
{
	int tId, gId;
	
	char** feature;
	int feature_count;
	int feature_len;

	int camId;
	shop_point3f_t pos;
};

struct qr_sensor_result_t
{
	int uID;
	int state;
};

struct static_counter_result_t
{
	int uID;
	int type;
	int diff;
};

struct gravity_sensor_result_t
{
	int uID;
	int diff;
};