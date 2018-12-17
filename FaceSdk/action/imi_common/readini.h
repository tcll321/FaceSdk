#ifndef _READ_INI_H
#define _READ_INI_H

//#define CONF_FILE_PATH  "Config.ini"

#include "TypesDef.h"
#include <string.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "TS_Log.h"
#ifndef WIN32
#include <unistd.h>
#ifndef stricmp
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif
#else
#define stricmp _stricmp
#define strnicmp _strnicmp
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif


class ReadIni
{
public:
#ifdef WIN32
	static int GetCurrentPath(char buf[], const char *pFileName);
#else
	static int GetCurrentPath(char buf[], const char *pFileName)
	{
		char pidfile[64];
		int bytes;
		int fd;
		sprintf(pidfile, "/proc/%d/cmdline", getpid());

		fd = open(pidfile, O_RDONLY, 0);
		bytes = read(fd, buf, 256);
		close(fd);
		buf[MAX_PATH] = '\0';

		char * p = &buf[strlen(buf)];
		do
		{
			*p = '\0';
			p--;
		} while ('/' != *p);
		p++;
		//配置文件目录
		memcpy(p, pFileName, strlen(pFileName));
		return 0;
	}
#endif

#if 0
	static int GetIniKeyString(const char *title, const char *key, const char* val, char* tmpstr, int len, const char *filename)
	{
		if (len <= 0 || !tmpstr)
			return 0;
		//static char tmpstr[1024];
		tmpstr[0] = '\0';
		int ret = GetPrivateProfileStringA(title, key, val ? val : "", tmpstr, len, filename);
		return ret;
	};
	static int GetIniKeyInt(const char *title, const char *key, int val, const char *filename)
	{

		return GetPrivateProfileIntA(title, key, val, filename);
	};
#else
	static std::vector<std::string> FindTitle(const char* filename, const char* part_title)
	{
		std::vector<std::string> titles;

		std::ifstream ifs(filename);
		if (!ifs.is_open())
		{
			TS_LogLog(ANDROID_LOG_INFO, "ReadINI", "no   such   file");
			return titles;
		}
		const std::string whitespace = " \t\f\v\n\r";

		char szLine[1024] = { 0 };

		while (!ifs.eof())
		{
			ifs.getline(szLine, 1024);
			int len = strlen(szLine);
			int real_len = 0;
			int scope = 0;
			for (int i = 0; i < len; i++)
			{
				if (szLine[i] == '(')
					scope++;
				else if (szLine[i] == ')')
					scope--;
				if (scope > 0 || whitespace.find_first_of(szLine[i]) == std::string::npos)
				{
					szLine[real_len++] = szLine[i];
				}
			}
			if (real_len < len)
				szLine[real_len] = '\0';

			if (szLine[0] != '[')
				continue;

			std::string s(szLine);
			int pos_comment = s.find("//");
			s = s.substr(0, pos_comment);

			char tmpstr[256];
			sprintf(tmpstr, "[%s_", part_title);

			int pos = s.find(tmpstr);
			if (pos != std::string::npos)
			{
				pos += strlen(tmpstr);
				int pos2 = s.find_last_of(']');
				std::string tail = s.substr(pos, pos2 - pos);
				titles.push_back(tail);
			}
		}
		return titles;
	}
	static int GetIniKeyString(const char *title, const char *key, const char* val, char* tmpstr, int len, const char *filename)
	{
		//FILE *fp;
		char szLine[1024];
		if (len <= 0 || !tmpstr)
		{
			return 0;
		}
		if (val)
			strcpy(tmpstr, val);
		else
			tmpstr[0] = '\0';
		//        int rtnval;
		//        int i = 0;
		int flag = 0;

		TS_LogLog(ANDROID_LOG_INFO, "ReadINI", "GetIniKeyString title(%s),key(%s),filename(%s)", title, key, filename);

		std::ifstream ifs(filename, std::ios_base::binary);
		if (!ifs.is_open())
			//        if((fp = fopen(filename, "r")) == NULL)
		{
			TS_LogLog(ANDROID_LOG_INFO, "ReadINI", "no   such   file");
			return strlen(tmpstr);
		}
		//        while(!feof(fp))

		const std::string whitespace = "\t\f\v\n\r";
		while (!ifs.eof())
		{
			ifs.getline(szLine, 1024);
			int len = strlen(szLine);
			int real_len = 0;
			//			int scope = 0;
			for (int i = 0; i < len; i++)
			{
				//				if(szLine[i] == '(')
				//					scope++;
				//				else if(szLine[i] == ')')
				//					scope--;
				if (//scope > 0 || 
					whitespace.find_first_of(szLine[i]) == std::string::npos)
				{
					szLine[real_len++] = szLine[i];
				}
			}
			if (real_len < len)
				szLine[real_len] = '\0';
			//			if(len > 0 && szLine[len - 1] == '\r')
			//				szLine[len - 1] = '\0';
#if 0
			rtnval = fgetc(fp);
			if (rtnval == EOF)
			{
				break;
			}
			else
			{
				szLine[i++] = rtnval;
			}
#endif
			if (1)//rtnval == '\n' || rtnval == '\r')
			{
				//                TS_LogLog(ANDROID_LOG_INFO,"ReadINI","hit line end");
				//                i--;
				//                szLine[--i] = '\0';
				//                i = 0;
				std::string s(szLine);
				int pos_comment = s.find(" //");
				s = s.substr(0, pos_comment);

				int pos = s.find_first_not_of(' ');
				if (pos == std::string::npos)
					continue;
				s = s.substr(pos);
				//				printf("substr(pos) = %s\n",s.c_str());
				int pos_tmp = s.find_first_of('=');//strchr(szLine, '=');
				if ((pos_tmp != std::string::npos) && (flag == 1))
				{
					if (1)
					{
						std::string cap = s.substr(0, pos_tmp);
						cap = cap.substr(0, cap.find_last_not_of(' ') + 1);
						//						printf("substr(0,pos_tmp) = %s\n",cap.c_str());
						if (stricmp(key, cap.c_str()) == 0 && (int)s.length() > pos_tmp + 1)//pos != szLine && (*(pos - 1) != ' ' && (*(pos - 1)) != '\t'))
						{
							//							printf("hit\n");
							s = s.substr(pos_tmp + 1);
							int p1 = s.find_first_not_of(' ');
							int p2 = s.find_last_not_of(' ');
							if (p1 == std::string::npos)
								break;
							s = s.substr(p1, p2 + 1 - p1);
							strcpy(tmpstr, s.c_str());
							//							printf("tmpstr = %s\n",tmpstr);
							TS_LogLog(ANDROID_LOG_INFO, "ReadINI", "Read String %s", tmpstr);
							return strlen(tmpstr);
						}
					}
				}
				else if (s.c_str()[0] == '[')
				{
					strcpy(tmpstr, "[");
					strcat(tmpstr, title);
					strcat(tmpstr, "]");
					if (strnicmp(tmpstr, s.c_str(), strlen(tmpstr)) == 0)
					{
						//找到title
						flag = 1;
						TS_LogLog(ANDROID_LOG_INFO, "ReadINI", "Find title (%s)", tmpstr);
					}
					else if (flag == 1)
						break;
				}
			}
		}
		//        fclose(fp);
		if (val)
			strcpy(tmpstr, val);
		else
			tmpstr[0] = '\0';

		return strlen(tmpstr);
	}

	//从INI文件读取整类型数据
	static int GetIniKeyInt(const char *title, const char *key, int val, const char *filename)
	{
		int res = val;
		char buff[1024];
		int len = GetIniKeyString(title, key, 0, buff, 1024, filename);
		if (len > 0)
			res = atoi(buff);
		return res;
	}
#endif


	template <typename T>
	static void load_param(const char* caption, const char* name, T& fl, const char* file)
	{
		//T fl;
		//		TS_Kernel::TS_LOCK lock(&m_crit);
		char buff[1024];
		int len = ReadIni::GetIniKeyString(caption, name, 0, buff, 1024, file);
		if (len)
			fl = (T)atof(buff);
		//		else
		//			fl = def;
		//		return fl;
	};

	static void load_string(const char* caption, const char* name, const char* def, const char* file, std::string& fl)
	{
		//		TS_Kernel::TS_LOCK lock(&m_crit);
		char buff[1024];
		int len = ReadIni::GetIniKeyString(caption, name, def, buff, 1024, file);
		if (len)
			fl = buff;
		else if (def)
			fl = def;
		else
			fl.clear();
	};

	typedef std::map<std::string, std::string> INI_SectionData;
	typedef std::map<std::string, INI_SectionData> INI_FileData;


	static std::string strip_string(std::string src, std::string space = "\t\n\r ")
	{
		std::string dst = src;
		//comment
		int pos = src.find("//");
		if (pos == 0)
			dst.clear();
		else if (pos > 0 && (src[pos - 1] == ' ' || src[pos - 1] == '\t'))
			dst = dst.substr(0, pos);
		int j = 0;
		for (int i = 0; i < dst.length(); i++)
		{
			if (space.find(dst[i]) != -1)
				continue;
			else
				dst[j++] = dst[i];
		}
		dst.resize(j);
		return dst;
	}

	static INI_SectionData parse_section(const char* caption, const char* file)
	{
		INI_SectionData res;
		std::ifstream ifs(file);
		if (!ifs.is_open())
			return res;
		
		std::string szCap = std::string("[") + caption + "]";

		char szStrip[1024];
		int sectionFlag = 0;
		while (!ifs.eof())
		{
			std::string szLine;
			std::getline(ifs, szLine);
			szLine = strip_string(szLine);
			if (!sectionFlag)
			{
				if (szLine.find(szCap) == 0)
					sectionFlag = 1;
				else
					continue;
			}
			else
			{
				if (szLine[0] == '[')
					break;

				int pos = szLine.find_first_of('=');
				if(pos == -1)
					continue;
				res[szLine.substr(0, pos)] = szLine.substr(pos + 1);
			}
		}
		return res;
	};
	static INI_FileData parse_file(const char* file)
	{
		INI_FileData res;
		std::ifstream ifs(file);
		if (!ifs.is_open())
			return res;

		while (!ifs.eof())
		{
			std::string szLine;
			std::getline(ifs, szLine);
			szLine = strip_string(szLine);
			if (szLine.empty())
				continue;
			if (szLine[0] == '[')
			{
				int pos = szLine.find_first_of(']');
				if (pos > 0)
				{
					std::string szCap = szLine.substr(1, pos - 1);
					res[szCap] = parse_section(szCap.c_str(), file);
				}
			}
		}
		return res;
	};

	static void write_section(std::ostream& ofs, const INI_SectionData& sData)
	{
		for (auto it = sData.begin(); it != sData.end(); it++)
		{
			ofs << it->first << " = " << it->second << std::endl;
		}
	};
	static std::ostream& write_file(std::ostream& ofs, const INI_FileData& fData)
	{
		for (auto it = fData.begin(); it != fData.end(); it++)
		{
			ofs << "[" << it->first << "]" << std::endl;
			write_section(ofs, it->second);
		}
		return ofs;
	};
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif

