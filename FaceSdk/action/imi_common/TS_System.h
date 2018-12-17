#ifndef TS_SYSTEM_H
#define TS_SYSTEM_H

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#include "TypesDef.h"
#ifndef __ANDROID__
#include <sys/syscall.h>
#endif
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
class TS_System
{
public:
	typedef struct TS_Time_
	{
		WORD year;
		WORD month;
		WORD day;

		WORD hour;
		WORD minute;
		WORD second;
	} TS_Time;
public:
	static void TS_Sleep(unsigned long t)
	{
#ifdef _WIN32
		Sleep(t);
#else
		usleep(t * 1000);
#endif
	}
	static TS_Time GetLocalTime(time_t rawtime = 0)
	{
		//		time_t rawtime;
		struct tm * timeinfo;
		/* get current timeinfo and modify it to the user's choice */
		if (!rawtime)
			time(&rawtime);
		timeinfo = localtime(&rawtime);
		TS_Time t;
		t.day = timeinfo->tm_mday;
		t.month = timeinfo->tm_mon + 1;
		t.year = timeinfo->tm_year + 1900;
		t.hour = timeinfo->tm_hour;
		t.minute = timeinfo->tm_min;
		t.second = timeinfo->tm_sec;
		return t;
	}
#ifdef _WIN32
	static bool GetModulePathName(std::string& strModulePathName)
	{
		//	CStdString szUdlFileName;
		char szModuleName[_MAX_PATH];
		char szDrive[_MAX_PATH];
		char szDir[_MAX_PATH];
		char szFilename[_MAX_PATH];
		char szExt[_MAX_PATH];
		char szNewFile[_MAX_PATH];
		char szFileData[_MAX_PATH];
		//            char szCutFile[_MAX_PATH];
		if (::GetModuleFileNameA(NULL, szModuleName, _MAX_PATH) == FALSE)
		{// 在这里要注意了，GetModuleFileName第一个参数为NULL表示获取的名字是当前的可执行文件的路径，如果你写的段代码在一个dll中，那么你要获取dll的路径，就必须把dll的
			// hModle传进来，可以见本文最后面的例子，告诉你怎么获取dll的hModule
			return false;
		}
		::ZeroMemory(szDrive, sizeof(szDrive));
		::ZeroMemory(szDir, sizeof(szDir));
		::ZeroMemory(szFilename, sizeof(szFilename));
		::ZeroMemory(szExt, sizeof(szExt));
		::ZeroMemory(szNewFile, sizeof(szNewFile));
		::ZeroMemory(szFileData, sizeof(szFileData));
		// split the path
		::_splitpath(szModuleName, szDrive, szDir, szFilename, szExt);
		strModulePathName = std::string(szDrive) + std::string(szDir);
		return true;
	}
	static bool GetModulePathName2(std::string& strModulePathName)
	{
		//strModulePathName = "/mnt/internal_sd/face-analysis";
		strModulePathName = "D:\\workspace\\face-analysis";
		return true;
	}
	static bool GetTempPathName(std::string & strTempPathName)
	{
		return GetModulePathName(strTempPathName);
	}
	static bool GetSavePathName(std::string & strTempPathName)
	{
		strTempPathName = "D:\\";
		return true;
	}

	static unsigned long GetTickCount()
	{
		return ::GetTickCount();
	}
#else
	static bool GetModulePathName2(std::string& strModulePathName)
	{
		char szModuleRoot[PATH_MAX];
#ifndef __ANDROID__
		pid_t pid = syscall(SYS_gettid);
#else
		pid_t pid = gettid();
#endif
		char link[PATH_MAX];
		sprintf(link, "/proc/%d/exe", pid);
		int n = readlink(link, szModuleRoot, PATH_MAX);
		if (n <= 0)
			return false;
		szModuleRoot[n] = '\0';

		//          int n = readlink("/proc/self/",szModuleRoot,PATH_MAX);
		strModulePathName = std::string(szModuleRoot);
		int p = strModulePathName.find_last_of('/');
		if (p != std::string::npos)
			strModulePathName = strModulePathName.substr(0, p);
		return true;
	}

	static bool GetModulePathName(std::string& strModulePathName)
	{
		//strModulePathName = "/mnt/internal_sd/face-analysis";
		strModulePathName = "/data/faceflow/face-analysis";
		return true;
	}

	static bool GetTempPathName(std::string & strTempPathName)
	{
		if (access("/sdcard", 0) >= 0)
			strTempPathName = "/sdcard";
		else
			strTempPathName = "/mnt/internal_sd/face-analysis";
		return true;
	}


	static bool GetSavePathName(std::string & strTempPathName)
	{
		if (access("/mnt/external_sd", 0) >= 0)
			strTempPathName = "/mnt/external_sd/face-analysis";
		else
			strTempPathName.clear();
		return true;
	}

	static unsigned long GetTickCount()
	{
		struct timespec ts;

		clock_gettime(CLOCK_MONOTONIC, &ts);

		return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
	}
#endif
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif