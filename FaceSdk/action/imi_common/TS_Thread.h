#ifndef __TS_THREAD_INC__
#define __TS_THREAD_INC__

#include<stdio.h>
#include<stdlib.h>
#include "TS_Log.h"
#include <string.h>
#include "TypesDef.h"
#ifndef WIN32
#include <unistd.h>
#include <sys/prctl.h>
#endif

#ifdef WIN32
#include "windows.h"
typedef void* (*ThreadFunc)(void*);

namespace TS_Kernel
{
	class TS_Event
	{
	public:
		TS_Event()
		{
			m_sem = CreateEvent(NULL, FALSE, 0, 0);
		}
		~TS_Event()
		{
			CloseHandle(m_sem);
		}
		inline void Set()
		{
			SetEvent(m_sem);
		}
		inline bool Check()
		{
			return (WaitForSingleObject(m_sem, 0) == WAIT_OBJECT_0);
		}
		inline bool Wait(DWORD t)
		{
			return (WaitForSingleObject(m_sem, t) == WAIT_OBJECT_0);
		}
	protected:
		HANDLE_T m_sem;
	};
    class TS_Thread
    {
    public:
		TS_Thread(ThreadFunc func, void* hdl, bool bLoop = true, float wait = 5)
            :m_bStart(false),m_thread(NULL)
        {
            m_func = func;
            m_hdl = hdl;
            m_wait = wait;
            m_bLoop = bLoop;

            m_sem = CreateEvent(NULL,FALSE,0,0);
			m_sem_sleep = CreateEvent(NULL, FALSE, 0, 0);
        }
        ~TS_Thread()
        {
            Stop();
            CloseHandle(m_sem);
			CloseHandle(m_sem_sleep);
        }

		int SetAffinity(DWORD_PTR cpu_mask)
		{
			if (!m_thread)
				return FALSE;  
	
			DWORD_PTR ret = SetThreadAffinityMask(m_thread, cpu_mask);
			return !ret;
		}

		void SetPeriod(int t)
		{
			m_wait = t;
		}

        int Start()
        {
            Stop();

            DWORD id;
            m_thread = CreateThread(NULL,0,WorkProc,(LPVOID)this,0,&id);

            if(m_thread == NULL)
            {
                return -1;
            }
            else
            {
                m_bStart = true;
                return 0;
            }
        }

		int Reset()
		{
			SetEvent(m_sem_sleep);
			return 0;
		}
        void Stop()
        {
            if(!m_bStart || !m_thread)
                return;
            SetEvent(m_sem);
			SetEvent(m_sem_sleep);
            if(WaitForSingleObject(m_thread,5000) != WAIT_OBJECT_0)
            {
                TerminateThread(m_thread,-1);
            }
            CloseHandle(m_thread);
            m_thread = NULL;
        }
protected:
        static DWORD WINAPI WorkProc(LPVOID hdl)
        {
            TS_Thread* p = (TS_Thread*)hdl;

            p->_WorkProc();
            return 0;
        }

        void _WorkProc()
        {
            if(!m_func)
                return;
            do
            {
                if(WaitForSingleObject(m_sem,0) == WAIT_OBJECT_0)
                {
                    break;
                }
                m_func(m_hdl);

				_Sleep();
//                Sleep(m_wait);
            }
            while (m_bLoop);
        }
		void _Sleep()
		{
			WaitForSingleObject(m_sem_sleep, m_wait);
		}

	private:
		ThreadFunc m_func;
		void* m_hdl;
		HANDLE_T m_thread;
		HANDLE_T m_sem;
		HANDLE_T m_sem_sleep;
		int m_wait;
		bool m_bLoop;
		bool m_bStart;
    };
}
#else
//#ifndef __ANDROID__
#include <sys/syscall.h>
//#endif
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
//#include <android/log.h>
#ifndef CPU_SET
#define NO_CPU_SET
#define CPU_SETSIZE 1024
#define __NCPUBITS  (8 * sizeof (unsigned long))
typedef struct
{
   unsigned long __bits[CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;

#define CPU_SET(cpu, cpusetp) \
  ((cpusetp)->__bits[(cpu)/__NCPUBITS] |= (1UL << ((cpu) % __NCPUBITS)))
#define CPU_ZERO(cpusetp) \
  memset((cpusetp), 0, sizeof(cpu_set_t))
#endif
typedef void* (*ThreadFunc)(void*);
namespace TS_Kernel
{
	class TS_Event
	{
	public:
		TS_Event()
		{
			sem_init(&m_sem,0,0);
		}
		~TS_Event()
		{
			sem_destroy(&m_sem);
		}
		inline void Set()
		{
			sem_post(&m_sem);
		}
		inline bool Check()
		{
			return (sem_trywait(&m_sem) == 0);
		}
		inline bool Wait(const struct timespec* ts = 0)
		{
			bool b = true;
			if(ts)
			{
				b = (sem_timedwait(&m_sem,ts) == 0 );
			}
			if(b)
			{
				while(sem_trywait(&m_sem) == 0)
				{}
			}
			return b;
		}
	protected:
		sem_t m_sem;
	};
    class TS_Thread
    {
    public:
        TS_Thread(ThreadFunc func,void* hdl,bool bLoop = true,float wait = 5):m_bStart(false)
        {
            m_func = func;
            m_hdl = hdl;
			m_wait_s = wait/1000;
            m_wait_ns = ((int)wait%1000)*1000000;
            m_bLoop = bLoop;
			m_name = 0;
        }
        ~TS_Thread()
        {
            Stop();
			if(m_name)
				delete m_name;
        }
		int Reset()
		{
			m_sem_sleep.Set();
			return 0;
		}
        int Start()
        {
            Stop();
			m_sem.Wait();
			int err = pthread_create(&m_thread,0,WorkProc,this);
            if(err == 0)
                m_bStart = true;
            return err;
        }
        void Stop()
        {
            if(!m_bStart)
                return;
            //sem_post(&m_sem);
			m_sem.Set();
			m_sem_sleep.Set();
            void * stat;
            pthread_join(m_thread,&stat);
            m_bStart = false;
			m_sem.Wait();
        }
		void SetPeriod(int t)
		{
			m_wait_s = t/1000;
			m_wait_ns = ((int)t%1000)*1000000;
		}
		int SetAffinity(cpu_set_t mask)
		{
			
			int ret = setCurrentThreadAffinityMask(mask);
			return ret;
		}
		void SetName(const char* name)
		{
			if(name)
			{
				m_name = new char[64];
				strcpy(m_name,name);
			}
		}
    protected:
		static void set_thread_policy(pthread_attr_t *attr,int policy)
		{
			int rs = pthread_attr_setschedpolicy(attr,policy);
			if (rs)
			{
				int err = errno;
				TS_LogLog_Print(ANDROID_LOG_ERROR,"TS_Thread","Error in the syscall pthread_attr_setschedpolicy: err=%d=0x%x", err, err);
			}
            else
            {
                TS_LogLog_Print(ANDROID_LOG_INFO,"TS_Thread","thread attr ok");
            }
//			assert(rs==0);
//			get_thread_policy(attr);
		}

        static void* WorkProc(void* hdl)
        {
            TS_Thread* p = (TS_Thread*)hdl;
            p->_WorkProc();
            pthread_exit((void *)0);
        }

        void _WorkProc()
        {
            if(!m_func)
                return;
			if(m_name)
				prctl(PR_SET_NAME,m_name);
			do
            {

                if(m_sem.Check())//sem_trywait(&m_sem) == 0)
                {
                    break;
				}
                m_func(m_hdl);
				_Sleep();
            }
            while (m_bLoop);
        }
		void _Sleep()
		{
			struct timespec ts,ts_sleep;
			if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
			{
				return;//throw error
			}
			ts_sleep.tv_sec = ts.tv_sec + m_wait_s;
			ts_sleep.tv_nsec = ts.tv_nsec + m_wait_ns;
			while(ts_sleep.tv_nsec >= 1000000000)
			{
				ts_sleep.tv_sec ++;
				ts_sleep.tv_nsec-= 1000000000;
			}
//			set_normalized_timespec(&ts_sleep,ts.tv_nsec + m_wait_ns,ts.tv_sec + m_wait_s);
			m_sem_sleep.Wait(&ts_sleep);
		}
        int setCurrentThreadAffinityMask(cpu_set_t mask)
        {
            int err, syscallres;
#ifndef __ANDROID__
			pid_t pid = syscall(SYS_gettid);//gettid();
#else
			pid_t pid = gettid();
#endif
#ifdef NO_CPU_SET
            syscallres = syscall(__NR_sched_setaffinity, pid, sizeof(mask), &mask);
#else
            syscallres = sched_setaffinity(pid,sizeof(mask),&mask);
#endif
            if (syscallres)
            {
                err = errno;
				TS_LogLog_Print(ANDROID_LOG_ERROR,"TS_Thread","Error in the syscall setaffinity: err=%d=0x%x,mask(%lu)", err, err, mask.__bits[0]);
            }
            else
            {
				TS_LogLog_Print(ANDROID_LOG_ERROR,"TS_Thread","setaffinity ok,mask(%lu)", mask.__bits[0]);
            }
			return syscallres;
        }
    private:
        ThreadFunc m_func;
        void* m_hdl;
        pthread_t m_thread;

		TS_Event m_sem;
		TS_Event m_sem_sleep;

		unsigned long m_wait_s;
		unsigned long m_wait_ns;
//                int m_wait;
        bool m_bLoop;
        bool m_bStart;

		char* m_name;
    };
}

#endif

#endif //__TS_LOCK_INC__
