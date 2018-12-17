#pragma once

template <typename T>
class Buffer
{
public:
	virtual void deposit(T data, int interval = 50) = 0;
	virtual bool fetch(T& f, int interval = 100, int step = 1) = 0;

	virtual bool fetch_back(T& f, int interval = 100) { return false; };
	virtual bool get_back(T& f, int interval = 100) { return false; };
	//static std::shared_ptr<Buffer<T> > create(int type);
};