#pragma once

#include <opencv2/opencv.hpp>
//#include "BasicStructs.h"
#include <mutex>
#include <condition_variable>
#include "Buffer.h"
template<typename T>
class CycleBuffer:public Buffer<T>
{
public:
	CycleBuffer(int _capacity = 500) :front(0), rear(0), count(0), capacity(_capacity){
		buffer = new cv::Mat[capacity];
	}
	~CycleBuffer() { delete [] buffer;}

	void deposit(cv::Mat data, int interval = 50)
	{
		std::unique_lock<std::mutex> l(lock);
		bool b = not_full.wait_for(l, std::chrono::microseconds(interval), [this]() {return count != capacity; });
		if (!b)
			return;

		//data.copyTo(buffer[rear]);
		buffer[rear] = data;
		rear = (rear + 1) % capacity;
		++count;
		//cv::imwrite(std::string("data/") + std::to_string(count) + ".jpg", m);
		//cv::imshow("m", m);
		//cv::waitKey(10);
		//l.unlock();
		not_empty.notify_one();
	}

	bool fetch(cv::Mat& f, int interval = 200)
	{
		std::unique_lock<std::mutex> l(lock);
		bool b = not_empty.wait_for(l,std::chrono::microseconds(interval), [this]() {return count != 0; });
		if (!b)
			return false;

		//buffer[front].copyTo(f);
		std::swap(f, buffer[front]);
//		f = buffer[front];

		front = (front + 1) % capacity;
		--count;

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
	cv::Mat * buffer;

	std::mutex lock;
	std::condition_variable not_full;
	std::condition_variable not_empty;
};