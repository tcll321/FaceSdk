#ifndef __TS_LOCK_INC__
#define __TS_LOCK_INC__

#if defined( WIN32)||defined(_WIN32) 
#include <windows.h>
namespace TS_Kernel
{
    class TS_LOCK;
	class TS_TIMED_LOCK;
    class TS_Mutex
    {
    friend class TS_LOCK;
	friend class TS_TIMED_LOCK;
    public:
        TS_Mutex(){
            InitializeCriticalSection(&m_crit);
        }
        ~TS_Mutex(){
            DeleteCriticalSection(&m_crit);
        }
    private:
        CRITICAL_SECTION m_crit;
    };
    class TS_LOCK
    {
    public:
        TS_LOCK(TS_Mutex* pSection){
            m_pSection = &pSection->m_crit;
            EnterCriticalSection(m_pSection);
        }
        ~TS_LOCK(){
            LeaveCriticalSection(m_pSection);
        }
    private:
        LPCRITICAL_SECTION m_pSection;
    };
	class TS_TIMED_LOCK
	{
	public:
		TS_TIMED_LOCK(TS_Mutex* pSection, int t) :m_pSection(0), bReady(false)
		{
			int count = t / 5;
			for (int i = 0; i < count; i++)
			{
				if (TryEnterCriticalSection(&pSection->m_crit))
				{
					bReady = true;
					break;
				}
			}
			if (bReady)
				m_pSection = &pSection->m_crit;
		}
		~TS_TIMED_LOCK(){
			if (bReady)
				LeaveCriticalSection(m_pSection);
		}
		bool IsReady(){
			return bReady;
		}
	private:
		bool bReady;
		LPCRITICAL_SECTION m_pSection;
	};
}
#else
#include <pthread.h>
namespace TS_Kernel
{
    class TS_LOCK;
    class TS_Mutex
    {
    friend class TS_LOCK;
    public:
        TS_Mutex(){
            pthread_mutex_init(&m_crit,0);
        }
        ~TS_Mutex(){
            pthread_mutex_destroy(&m_crit);
        }
    private:
        pthread_mutex_t m_crit;
    };
    class TS_LOCK
    {
    public:
        TS_LOCK(TS_Mutex* pSection){
            m_pSection = &pSection->m_crit;
            pthread_mutex_lock(m_pSection);
        }
        ~TS_LOCK(){
            pthread_mutex_unlock(m_pSection);
        }
    private:
        pthread_mutex_t* m_pSection;
    };
}
#endif

#endif //__TS_LOCK_INC__
