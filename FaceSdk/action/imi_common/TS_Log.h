//
// Created by Administrator on 2016/12/13.
//

#ifndef FLOWFACETEST_LOG_H
#define FLOWFACETEST_LOG_H

#include <string>
#include <fstream>

#define TS_DEBUG   0
#ifdef __ANDROID__
#define ANDROID_LOG 1
#else
#define ANDROID_LOG 0
#endif
//#include <glog/logging.h>

#ifdef _WIN32
#include <windows.h>
#define ANDROID_LOG_UNKNOWN 0
#define ANDROID_LOG_DEFAULT 1    /* only for SetMinPriority() */
#define ANDROID_LOG_VERBOSE 2
#define ANDROID_LOG_DEBUG 3 
#define ANDROID_LOG_INFO 4
#define ANDROID_LOG_WARN 5
#define ANDROID_LOG_ERROR 6
#define ANDROID_LOG_FATAL 7
#define ANDROID_LOG_SILENT 8

#define my_android_log_print(x,y,...) //TS_LogLog(x,y,z,##__VA_ARGS__) 
#else
#include <unistd.h>
#if ANDROID_LOG
#include <android/log.h>
#define my_android_log_print __android_log_print
#else
#define my_android_log_print(x,y,...) 
#endif
#endif
#include "TS_System.h"
#ifdef WIN32
#define WRITE_FOLDER "./internal_sd/"
#else
#define WRITE_FOLDER "/mnt/internal_sd/FaceFlowLog/"
#endif

#if 0
#define TS_LogDebug_(level,module,fmt,...) \
				{	\
		char message_[256];\
		sprintf(message_,"%s "#fmt,module,##__VA_ARGS__);\
		int severity = 0; \
		switch(level) \
								{ \
		case ANDROID_LOG_SILENT: \
			severity = google::INFO;\
			break; \
		case ANDROID_LOG_FATAL: \
			severity = google::FATAL;\
			break;\
		case ANDROID_LOG_ERROR:\
			severity = google::LOG_ERROR;\
			break;\
		case ANDROID_LOG_WARN:\
			severity = google::WARNING;\
			break;\
		default:\
			severity = google::INFO;\
			break;\
		}; \
		LOG(severity)<<message_;\
	}
#endif
#define TS_LogDebug_(level,module,fmt,...) \
	{	\
		char message_[256];\
		printf("%s "#fmt"\n",module,##__VA_ARGS__);\
				}
#ifdef WIN32
#define TS_LogDebugFile_(file,level,module,fmt,...) \
	{	\
		char message_[256];\
		TS_System::TS_Time ttt = TS_System::GetLocalTime(); \
		sprintf(message_, "%s %04d-%02d-%02d %02d:%02d:%02d "#fmt"\n",module, ttt.year, ttt.month, ttt.day, ttt.hour, ttt.minute, ttt.second, ##__VA_ARGS__);\
		std::ofstream ofs(file,std::ios_base::app);\
		if(ofs.is_open()) \
		ofs << message_;\
	}
#else
#define TS_LogDebugFile_(file,level,module,fmt,...) \
		{	\
		char message_[256];\
		TS_System::TS_Time ttt = TS_System::GetLocalTime(); \
		sprintf(message_, "%s %04d-%02d-%02d %02d:%02d:%02d "#fmt"\n",module, ttt.year, ttt.month, ttt.day, ttt.hour, ttt.minute, ttt.second, ##__VA_ARGS__);\
		std::ofstream ofs(file,std::ios_base::app);\
		ofs << message_;\
		}
#endif
#if TS_DEBUG
#define TS_LogDebug TS_LogDebug_
#else
#define TS_LogDebug(level,module,fmt,...) 
#endif

#define TS_LogLogGlobal(level,module, fmt,...) \
	{\
		TS_LogDebug(level,module,fmt,##__VA_ARGS__);\
		my_android_log_print(level, module, fmt,##__VA_ARGS__); \
	}	

#define TS_LogLog TS_LogLogGlobal
#define TS_LogLog_Print(level,module, fmt,...) \
				{\
		TS_LogDebug_(level,module,fmt,##__VA_ARGS__);\
		my_android_log_print(level, module, fmt,##__VA_ARGS__); \
				}	
#define TS_LogLog_File(file,level,module,fmt,...) \
	{\
		TS_LogDebugFile_(file, level,module,fmt,##__VA_ARGS__);\
		my_android_log_print(level, module, fmt,##__VA_ARGS__); \
	}
#endif //FLOWFACETEST_LOG_H
