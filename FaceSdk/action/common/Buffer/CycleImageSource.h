#pragma once

//#include "QueueBuffer.h"
//#define USE_A100 1
#include "ImageSource.h"
#include "CycleImageBuffer.h"

class CycleImageSource
{
public:
	CycleImageSource(int _cap, std::string type, std::string path, std::string flag):m_buffer(_cap)
	{
		m_bWorking = false;
		if (!type.empty())
		{
			open(type, path, flag);
		}
	};

	~CycleImageSource()
	{
		close();
	}
	bool open(std::string type, std::string path, std::string flag)
	{
		if (m_bWorking)
			close();
		m_pSource.reset(new ImageSource(type, path, flag));

		m_bWorking = true;
		m_thread = std::thread(&CycleImageSource::_proc, this);
		return true;
	};
	void close()
	{
		m_bWorking = false;
		m_thread.join();
		m_pSource.release();
	};

	void _proc()
	{
		while (m_bWorking)
		{
			cv::Mat m;
			std::string str;
			m_pSource->next(m, str);
			if (!m.empty())
			{
				shop::Frame f(m, 0);
				m_buffer.deposit(f);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}


	Buffer<cv::Mat>* getBuffer() {
		return (Buffer<cv::Mat>* )(&m_buffer);
	};

	std::auto_ptr<ImageSource> m_pSource;
	CycleImageBuffer m_buffer;
	std::thread m_thread;
	bool m_bWorking;
};