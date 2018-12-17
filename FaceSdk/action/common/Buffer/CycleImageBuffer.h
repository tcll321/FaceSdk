#pragma once

#include <opencv2/opencv.hpp>
//#include "BasicStructs.h"
#include <mutex>
#include <condition_variable>
#include "../common/Buffer/Buffer.h"
#include "../common/BasicStructs.h"

//template<typename T>

class CycleImageBuffer:public Buffer<shop::Frame>
{
public:
	CycleImageBuffer():front(0), rear(0), count(0), 
		capacity(0), buffer(0), 
		fps(0), bOverlap(false),bWait(false),
		last_stamp(0), stamp_on_time(0), stamp_step(0), stamp_thresh(0)
	{}
	CycleImageBuffer(int _capacity, float _fps = 0, bool _bOverlap = false, bool _wait = 0) :front(0), rear(0), count(0),
		last_stamp(0), stamp_on_time(0), stamp_step(0), stamp_thresh(0)
	{
		init(_capacity, _fps, _bOverlap, _wait);
	}

	void init(int _capacity, float _fps = 0, bool _bOverlap = false, bool _bWait = false)
	{
		capacity = _capacity;
		buffer = new shop::Frame[capacity];
		bOverlap = _bOverlap;
		bWait = _bWait;
		if (fps > 0)
		{
			stamp_step = 1000.0 / fps;
			stamp_thresh = stamp_step / 4;
		}
	}

	~CycleImageBuffer() { delete [] buffer;}

	void deposit(shop::Frame data, int interval = 50)
	{
		std::unique_lock<std::mutex> l(lock);

		if (fps > 0 && stamp_on_time - stamp_thresh > data.meta.t)
			return;
	
		if (!bOverlap)
		{
			bool b = not_full.wait_for(l, std::chrono::microseconds(interval), [this]() {return count != capacity; });
			if (!b)
				return;
		}
		if (fps > 0)
		{
			do {
				stamp_on_time += stamp_step;
			} while (stamp_on_time >= data.meta.t);
			last_stamp = data.meta.t;
		}

		//buffer[rear] = data;
		data.m.copyTo(buffer[rear].m);
		buffer[rear].meta = data.meta;
		rear = (rear + 1) % capacity;
		++count;
		not_empty.notify_one();
	}

	bool fetch(shop::Frame& frame, int interval = 200, int step = 1)
	{
		if (!count)
			return false;
		std::unique_lock<std::mutex> l(lock);

		bool b = not_empty.wait_for(l, std::chrono::microseconds(bWait? 5000:interval), [this]() {return count != 0; });
			if (!b)
				return false;
		//frame = buffer[front];
		frame.meta = buffer[front].meta;
		buffer[front].m.copyTo(frame.m);

		front = (front + 1) % capacity;
		--count;

		//std::cout << "sz = " << count << std::endl;
		//l.unlock();
		not_full.notify_one();

		return true;
	}

	bool fetch_near(shop::Frame&frame, double stamp, int interval = 200)
	{
		std::unique_lock<std::mutex> l(lock);
		bool b = not_empty.wait_for(l, std::chrono::microseconds(bWait ? 5000 : interval), [this]() {return count != 0; });
		if (!b)
			return false;
	
		double diff = abs(buffer[front].meta.t - stamp);
		
		int tCount = count;
		tCount--;

		int tPrevFront = front;
		int tFront = (front + 1) % capacity;

		double tDiff = 0;
		while (tCount && (tDiff = abs(buffer[tFront].meta.t - stamp)) < diff)
		{
			tCount--;
			tPrevFront = tFront;
			tFront = (tFront + 1) % capacity;
		}

		frame.meta = buffer[tPrevFront].meta;

		buffer[tPrevFront].m.copyTo(frame.m);

		front = tFront;
		count = tCount;
		//std::cout << "sz = " << count << std::endl;
		//l.unlock();
		not_full.notify_one();

		return true;
	}
	bool is_empty() const 
	{
		return !count;
	}
private:
	int front;
	int rear;
	int count;
	int capacity;
	
	float fps;
	bool bOverlap;
	bool bWait;
	
	shop::Frame * buffer;

	double stamp_on_time;
	double last_stamp;
	double stamp_step;
	double stamp_thresh;

	std::mutex lock;
	std::condition_variable not_full;
	std::condition_variable not_empty;
};