#pragma once

#include <fstream>
#include <string>

#include <algorithm>
#include <iostream>

#include <vector>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
class ListFiles
{
public:
	static int process(std::string folder, std::string save_file, bool bRecursive = false, std::string append = ".jpg|.jpeg|.png|.bmp",  std::string prefix = "")
	{
		using namespace std;
		
		if (prefix.empty())
		{
			ofstream ofs(save_file, std::ios_base::binary);
			ofs.close();
		}
		std::vector<std::string> vappend;
		if (1)
		{
			std::stringstream ss(append);
			std::string item;
			while (std::getline(ss, item, '|'))
			{
				vappend.push_back(std::move(item));
			}
		}


		WIN32_FIND_DATAA file_data;
		HANDLE hdl = FindFirstFileA((folder + "/" + prefix + "/*.*").c_str() , &file_data);
		if (hdl == INVALID_HANDLE_VALUE)
			return -1;
		bool bNext = true;
		while (bNext)
		{
			if (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && bRecursive)
			{
				if (file_data.cFileName[0] != '.')
					process(folder, save_file, true, append, (prefix.empty()?"":(prefix + "/")) + file_data.cFileName);
			}
			else
			{
				std::string szFile = prefix.empty() ? file_data.cFileName : (prefix + "/" + file_data.cFileName);
				std::transform(szFile.begin(), szFile.end(), szFile.begin(), tolower);
				
				bool bFound = false;
				if (vappend.empty())
					bFound = true;
				else
				{
					for(int k = 0;k < vappend.size();k++)
						if (szFile.find(vappend[k]) != -1)
						{
							bFound = true;
							break;
						}
				}
				if (bFound)//szFile.find(".jpg") != -1 || szFile.find(".jpeg") != -1 || szFile.find(".png") != -1 || szFile.find(".bmp") != -1)
				{

//					std::cout << szFile.c_str() << "," << file_data.cAlternateFileName << std::endl;
//					if (szFile.find("齐秦") != -1)
					{
						ofstream ofs(save_file, ios_base::app);
						ofs << szFile.c_str() << std::endl;
					}

				}
			}
			bNext = FindNextFileA(hdl, &file_data);
		}
		return 0;
	};
	static int create_folder(std::string filename)
	{
		if (filename.empty())
			return -1;
		std::string szFolder = filename;
		if (filename.find_last_of(".") != -1 && filename[0] != '.')
		{
			szFolder = filename.substr(0, filename.find_last_of("/\\"));
			szFolder = szFolder.substr(0, szFolder.find_last_not_of("/\\") + 1);
		}
		check_and_create(szFolder);
		return 0;
	}

	static bool check_and_create(std::string folder)
	{
		if (!exists(folder))
		{
			int pos = folder.find_last_of("/\\");
			if (pos != -1)
			{
				std::string szParent = folder.substr(0, pos);
				szParent = szParent.substr(0, szParent.find_last_not_of("/\\") + 1);
				check_and_create(szParent);
			}
		}
		return CreateDirectoryA(folder.c_str(), NULL);
	};

	static bool exists(std::string dirName_in)
	{
		int ftyp = _access(dirName_in.c_str(), 0);

		if (0 == ftyp)
			return true;   // this is a directory!  
		else
			return false;    // this is not a directory!  
	}

	static std::vector<std::string> read(std::string list_file, std::string path)
	{
		if (!exists(list_file))
			return std::vector<std::string>();

		std::vector<std::string> v;

		std::ifstream ifs(list_file);
		if (!ifs.is_open())
			return v;

		while (!ifs.eof())
		{
			std::string line;
			ifs >> line;
			if (line.empty())
				continue;

			v.push_back(path.empty() ? line : (path + "/" + line));
		}
		return v;
	}
};
#else
#include <sys/io.h>
#include <dirent.h>
#include <ftw.h>
#include <string.h>

class ListFiles
{
public:
	static int process(std::string dir, std::string save_file, bool bRecursive = false, std::string append = ".jpg|.jpeg|.png|.bmp", std::string prefix = "")
	{
		using namespace std;

		if (prefix.empty() && !save_file.empty())
		{
			ofstream ofs(save_file);
			ofs.close();
		}
		std::vector<std::string> vappend;
		if (1)
		{
			std::stringstream ss(append);
			std::string item;
			while (std::getline(ss, item, '|'))
			{
				vappend.push_back(std::move(item));
			}
		}


		std::string dirNew = dir + "/" + prefix + "/*.*";

		DIR* handle;
		struct dirent* filename;
		handle = opendir(dirNew.c_str());
#if 0
		intptr_t handle;
		_finddata_t findData;

		handle = _findfirst(dirNew.c_str(), &findData);

		if (handle == -1)        // 检查是否成功
			return;
#else
		if (!handle)
			return;
#endif
		//do
		while(filename = readdir(handle))
		{
//			if (findData.attrib & _A_SUBDIR)
			if(filename->d_type == DT_DIR)
			{
				//if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				if(strcmp(filename->d_name, ".") == 0 || strcmp(filename->d_name, "..") == 0)
					continue;

				//cout << findData.name << "\t<dir>\n";

				// 在目录后面加上"\\"和搜索到的目录名进行下一次搜索
				if (bRecursive)
				{
					//std::string newPrefix = prefix + "/" + findData.name;
					std::string newPrefix = prefix + "/" + filename->d_name;
					process(dir, save_file, bRecursive,append, newPrefix);
				}
			}
			else
			{
				std::string szFile = prefix.empty() ? filename->d_name : (prefix + "/" + filename->d_name);
				//std::string szFile = prefix.empty() ? findData.name : (prefix + "/" + findData.name);
				std::transform(szFile.begin(), szFile.end(), szFile.begin(), ::tolower);

				bool bFound = false;
				if (vappend.empty())
					bFound = true;
				else
				{
					for (int k = 0; k < vappend.size(); k++)
						if (szFile.find(vappend[k]) != -1)
						{
							bFound = true;
							break;
						}
				}
				if (bFound)//szFile.find(".jpg") != -1 || szFile.find(".jpeg") != -1 || szFile.find(".png") != -1 || szFile.find(".bmp") != -1)
				{

					//					std::cout << szFile.c_str() << "," << file_data.cAlternateFileName << std::endl;
					//					if (szFile.find("齐秦") != -1)
					{
						ofstream ofs(save_file, ios_base::app);
						ofs << szFile.c_str() << std::endl;
					}

				}
			}
			//	cout << findData.name << "\t" << findData.size << " bytes.\n";
		}
		;
		//while (_findnext(handle, &findData) == 0);

		//_findclose(handle);    // 关闭搜索句柄
		closedir(handle);
		return 0;
	}

	static int create_folder(const char *pszDirectory)
	{
		char szRunCmd[256] = { 0 };
		if (pszDirectory == NULL)
		{
			printf("create_folder: input parameter isNULL!\n");
			return -1;

		}
		sprintf(szRunCmd, "mkdir -p %s", pszDirectory);
		system(szRunCmd);
		return 0;
	}

	static bool exists(std::string dirName_in)
	{
		FILE* p = fopen(dirName_in.c_str(), "r");
		if (!p)
			return false;
		else
		{
			fclose(p);
			return true;
		}
#if 0
		int ftyp = _access(dirName_in.c_str(), 0);

		if (0 == ftyp)
			return true;   // this is a directory!  
		else
			return false;    // this is not a directory!  
#endif
	}

	static bool check_and_create(std::string folder)
	{
		if (!exists(folder))
		{
			int pos = folder.find_last_of("/\\");
			if (pos != -1)
			{
				std::string szParent = folder.substr(0, pos);
				szParent = szParent.substr(0, szParent.find_last_not_of("/\\") + 1);
				check_and_create(szParent);
			}
		}
		return create_folder(folder.c_str());
	};

};
#endif