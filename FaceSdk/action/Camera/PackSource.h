#pragma once
#include <ImiSkeleton.h>
#include "ImageSource.h"
#include <memory>
#include <thread>
#include <atomic>
class PackImageSource
{
public:
	PackImageSource() {};
	~PackImageSource() { _stop(); }
	void open_a100(int id)
	{
		_stop();
		ImageSource* p = new ImageSource("a100", "27", std::to_string(id));
		m_pColorSource = std::shared_ptr<ImageSource>(p);
		m_pDepthSource = m_pColorSource;
		bSkeleton = true;
		bHoldMode = false;
		_start();
	};
	void open_folder(const char* folder)
	{
		_stop();
		m_pColorSource = std::make_shared<ImageSource>("folder", "", folder, true, ".jpg");
		m_pDepthSource = std::make_shared<ImageSource>("folder", "", folder, true, ".dat");
		bSkeleton = false;
		bHoldMode = true;
		_start();
	};

	bool get(std::vector<cv::Mat>& mv, ImiSkeletonFrame* sk, int interval = 500/*ms*/)
	{
		m_countDown = 2;
		mv.resize(2);
		m_pmColor = &mv[0];
		if(bSkeleton)
			m_pSkeleton = sk;
		m_pmDepth = &mv[1];

		do {
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		} while (m_countDown);

		return true;
	};


	void _proc_depth()
	{
		while (bWorking)
		{
			bool bWrite = (cv::Mat*)(m_pmDepth);
			cv::Mat* pm = 0;
			cv::Mat _m;

			if (bWrite)
			{
				pm = (cv::Mat*)(m_pmDepth);
			}
			else if(!bHoldMode)
			{
				pm = &_m;
			}
			if (pm)
			{
				std::string szName;
				m_pDepthSource->next(*pm, szName, 2, m_pSkeleton);
				//std::cout << "m.size = " << pm->size() << std::endl;
				if (bWrite && !pm->empty())
				{
					m_pSkeleton = 0;
					m_pmDepth = 0;
					m_countDown--;
				}
			}
		
			//else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
		}
	};
	void _proc_color()
	{
		while (bWorking)
		{
			bool bWrite = (cv::Mat*)(m_pmColor);
			cv::Mat* pm = 0;
			cv::Mat _m;

			if (bWrite)
			{
				pm = (cv::Mat*)(m_pmColor);
			}
			else if(!bHoldMode)
			{
				pm = &_m;
			}

			if (pm)
			{
				std::string szName;
				m_pDepthSource->next(*pm, szName, 1);
				//std::cout << "m.size = " << pm->size() << std::endl;
				if (bWrite && !pm->empty())
				{
					m_pmColor = 0;
					m_countDown--;
				}
			}
			//else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	};

	void _start()
	{
		bWorking = true;
		m_thread[0] = std::thread(&PackImageSource::_proc_color, this);
		m_thread[1] = std::thread(&PackImageSource::_proc_depth, this);
	};
	void _stop()
	{
		bWorking = false;
		if (m_thread[0].joinable())
			m_thread[0].join();
		if (m_thread[1].joinable())
			m_thread[1].join();
	};
	std::shared_ptr<ImageSource> m_pColorSource;
	std::shared_ptr<ImageSource> m_pDepthSource;
	bool bSkeleton = false;
	bool bHoldMode = false;
	bool bWorking = false;

	std::thread m_thread[2];
	std::atomic<cv::Mat*> m_pmColor = 0, m_pmDepth = 0;
	std::atomic<ImiSkeletonFrame*> m_pSkeleton = 0;
	std::atomic<int> m_countDown = 0;
};