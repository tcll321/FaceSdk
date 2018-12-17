#pragma once
#pragma once

#include <opencv2/opencv.hpp>
//#include "BasicStructs.h"
#include <mutex>
#include <condition_variable>
#include <deque>
#include "../common/Buffer/Buffer.h"

template<typename T>
class QueueBuffer:public Buffer<T>
{
public:
	QueueBuffer() {}
	QueueBuffer(int capicity){
		m_capicity = capicity;
	}
	~QueueBuffer(){}

	void deposit(T data, int deposit = 50)
	{
		std::unique_lock<std::mutex> l(lock);
		//not_full.wait(l, [this]() {return count != capacity; });
		//buffer[rear] = data;

		buffer.push_back(data);
		++count;

		while (count > m_capicity)
		{
			buffer.pop_front();
			count--;
		}
		//printf("queue buffer deposit, size = %d\n", count);
		//cv::imwrite(std::string("data/") + std::to_string(count) + ".jpg", m);
		//cv::imshow("m", m);
		//cv::waitKey(10);

		//l.unlock();
		not_empty.notify_one();
	}

	bool fetch(T& f, int interval = 100, int step = 1)
	{
		std::unique_lock<std::mutex> l(lock);
		bool b = not_empty.wait_for(l, std::chrono::microseconds(interval), [this]() {return !buffer.empty(); });
		if (!b)
			return false;
		while (step-- && count)
		{
			f = buffer.front();
			buffer.pop_front();

			--count;
		}
	
		//not_full.notify_one();
		return true;
	}

	bool fetch_back(T& f, int interval = 100)
	{
		std::unique_lock<std::mutex> l(lock);
		bool b = not_empty.wait_for(l, std::chrono::microseconds(interval), [this]() {return !buffer.empty(); });
		if (!b)
			return false;
		f = buffer.back();
		buffer.clear();
		count = 0;
		return true;
	}

	bool get_back(T& f, int interval = 100)
	{
		std::unique_lock<std::mutex> l(lock);
		bool b = not_empty.wait_for(l, std::chrono::microseconds(interval), [this]() {return !buffer.empty(); });
		if (!b)
			return false;
		f = buffer.back();
		//buffer.clear();
		//count = 0;
		return true;
	}

	int count = 0;
	std::deque<T> buffer;

	std::mutex lock;
	//std::condition_variable not_full;
	std::condition_variable not_empty;
	int m_capicity = 50;
};