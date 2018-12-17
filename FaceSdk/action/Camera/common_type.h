#pragma once

typedef struct rect_t
{
	int x, y, w, h;
};

typedef struct object_t
{
	int type;
	float prob;
	//	float* vProb;
}; 

struct image_t
{
	unsigned char* data;
	int w, h;
};

struct tracker_t
{
	float x, y, z;
	int state;
	int id;
};

struct cart_tracker_result_t
{
	float body_x, body_y, body_z;
	float hand_x, hand_y, hand_z;
	int direction;
	int type;
	float score;
};

struct goods_single_t
{
	int type;
	int count;
};

struct cart_state_t
{
	int id;
	int goods_count;
	goods_single_t* goods_data;
};

struct face_info_t
{
	int tId, gId;
	char* feature;

	int camId;
	int pos_x, pos_y, pos_z;
};