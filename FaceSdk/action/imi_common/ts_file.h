#ifndef TS_FILE_H
#define TS_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include "TypesDef.h"
#ifdef WIN32
#include <windows.h>
#include <io.h>
#include <ShlObj.h>
#pragma comment(lib,"shell32.lib")
#else
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif


namespace TS_File
{
	enum PROG_ERRCODES {
		OP_OK = 0,
		OP_BAD_FIRST_FILE = 1 << 1,
		OP_BAD_DRIVESTRINGS = 1 << 2,
		OP_BAD_LAST_FIND_FILE = 1 << 3,
		OP_BAD_FIND_CLOSE = 1 << 4,
		OP_FAILED = 0xffffffff
	};

	typedef bool(*FileProc)(const char* fullname, const char* subname, bool bDir, void* hdl);
#ifdef WIN32
	static enum PROG_ERRCODES walkDirectoryTree(const char* rootDirectory, FileProc proc, void* hdl, int depth = -1, const char* prefix = NULL)
	{
		static const char charSuffix[] = "\\*";
		static const int charSuffixLen = 2;
		static const char symbolicDirName[] = ".";
		static const char symbolicParentDirName[] = "..";

		WIN32_FIND_DATAA fData;
		HANDLE_T hFind;
		char rootDirectoryStar[MAX_PATH];
		sprintf_s(rootDirectoryStar, "%s%s", rootDirectory, charSuffix);
		//		printf("D %s\n", rootDirectoryStar);

		if ((hFind = FindFirstFileA(rootDirectoryStar, &fData)) == INVALID_HANDLE_VALUE) {
			return OP_BAD_FIRST_FILE;
		}

		do {
			int cFileNameLen = strlen(fData.cFileName);
			int rootDirectoryLen = strlen(rootDirectory);

			/* skip the symbolic directories */
			if (strcmp(fData.cFileName, symbolicParentDirName) == 0 || strcmp(fData.cFileName, symbolicDirName) == 0) {
				continue;

				/* if we're dealing with a directory */
			}
			else if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (depth == 0)
					continue;
				int nextRootDirectoryLen =
					(rootDirectoryLen)+1 +/* length of root directory, minus one */
					cFileNameLen +                /* length of leaf directory */
					//charSuffixLen +               /* for slash star */
					1;                            /* for null terminator */
				enum PROG_ERRCODES err;
				char* nextRootDirectory = (char*)malloc(nextRootDirectoryLen);

				memset(nextRootDirectory, '\0', nextRootDirectoryLen);
				strncpy(nextRootDirectory, rootDirectory, rootDirectoryLen);
				nextRootDirectory[rootDirectoryLen] = '\\';
				strncat(nextRootDirectory, fData.cFileName, cFileNameLen);
				//strncat(nextRootDirectory, charSuffix, charSuffixLen);
				proc(nextRootDirectory, fData.cFileName, true, hdl);
				if (!(err = walkDirectoryTree(nextRootDirectory, proc, hdl, depth - 1, prefix))) {
					/* something went wrong */
				}

				free(nextRootDirectory);

				/* otherwise treat it as a file */
			}
			else {
				int cFilePathLen = rootDirectoryLen + 1 + cFileNameLen + 1;
				char* cFilePath = (char*)malloc(cFilePathLen);
				//enum PROG_ERRCODES err;

				/* concatenate root directory star minus the star and cfile name
				* then call processFile(..) with the newly formed string */

				memset(cFilePath, '\0', cFilePathLen);
				strncpy(cFilePath, rootDirectory, rootDirectoryLen);
				cFilePath[rootDirectoryLen] = '\\';
				strncat(cFilePath, fData.cFileName, cFileNameLen);

				if ((!prefix || strstr(fData.cFileName, prefix)) && !proc(cFilePath, fData.cFileName, false, hdl))//, (const WIN32_FIND_DATA* const)&fData))) {
				{
					/* something went wrong */
				}

				free(cFilePath);
			}
		} while (FindNextFileA(hFind, &fData));

		if (GetLastError() != ERROR_NO_MORE_FILES) {
			if (!FindClose(hFind)) {
				return OP_BAD_LAST_FIND_FILE;
			}
			else {
				return OP_BAD_FIND_CLOSE;
			}
		}

		if (!FindClose(hFind)) {
			return OP_BAD_FIND_CLOSE;
		}
		return OP_OK;
	}
#else
	static enum PROG_ERRCODES walkDirectoryTree(const char* folder, FileProc proc, void* hdl, int depth = -1, const char* prefix = NULL)
	{
		char* dir = (char*)folder;

		DIR *dp;
		struct dirent *entry;
		struct stat statbuf;

		if ((dp = opendir(dir)) == NULL) {
			fprintf(stderr, "Can`t open directory %s\n", dir);
			return OP_BAD_FIRST_FILE;
		}

		//chdir(dir);
		while ((entry = readdir(dp)) != NULL) {
			lstat(entry->d_name, &statbuf);
			if (entry->d_type == DT_DIR || S_ISDIR(statbuf.st_mode)) {
				if (strcmp(entry->d_name, ".") == 0 ||
					strcmp(entry->d_name, "..") == 0)
					continue;
				if (depth == 0)
					continue;
				//printf("%*s%s/\n", depth, "", entry->d_name);
				char temp_folder[256];
				sprintf(temp_folder, "%s/%s", folder, entry->d_name);

				proc(temp_folder, entry->d_name, true, hdl);
				walkDirectoryTree(temp_folder, proc, NULL, depth - 1, prefix);//printdir(entry->d_name, depth + 4);
			}
			else
			{
				//if(strcmp(entry->d_name,"001") != 0)
				//{
				if (!prefix || strstr(entry->d_name, prefix))//(statbuf.st_mode&S_IROTH) == 0 || (statbuf.st_mode&S_IWOTH) == 0)
				{
					char temp_file[256];
					sprintf(temp_file, "%s/%s", folder, entry->d_name);
					proc(temp_file, entry->d_name, false, hdl);
					//proc(temp_file);//, statbuf.st_mode | S_IROTH | S_IWOTH);
					//					printf("changeMode %s\n", entry->d_name);
				}
				//}

				//				printf("%*s%s\n", depth, "", entry->d_name);
			}
		}
		//chdir("..");
		closedir(dp);
		return OP_OK;
	}
#endif
	static bool Delete(const char* file)
	{
#ifdef WIN32
		return DeleteFileA(file) == TRUE;
#else
		return remove(file) == 0;
#endif
	}
	static bool FileProc_Remove(const char* fullname, const char* subname, bool bDir, void* hdl)
	{
		if (bDir)
			return true;
#ifdef WIN32
		return DeleteFileA(fullname) == TRUE;
#else
		return remove(fullname) == 0;
#endif
	}

	static bool CreateFolder(const char* folder)
	{
#ifdef WIN32
		if (_access(folder, 0) >= 0)
		{
			//			printf("create folder %s exists\n",folder);
			return true;
		}
		std::vector<char> tFolder(strlen(folder) + 1);
		for (size_t i = 0; i < strlen(folder); i++)
		{
			if (folder[i] == '/')
				tFolder[i] = '\\';
			else
				tFolder[i] = folder[i];
		}
		tFolder[strlen(folder)] = '\0';
		return SHCreateDirectoryExA(NULL, &tFolder[0], NULL) == 0;
		//		TS_File::EmptyDirectory(m_szTempFolder.c_str());
		//RemoveDirectoryA(m_szTempFolder.c_str());
#else
		if (access(folder, 0) >= 0)
			return true;
		std::vector<char> tFolder(strlen(folder) + 1);
		for (int i = 0; i < strlen(folder); i++)
		{
			if (folder[i] == '\\')
				tFolder[i] = '/';
			else
				tFolder[i] = folder[i];
		}
		tFolder[strlen(folder)] = '\0';
		char command[256];
		sprintf(command, "mkdir -p %s", &tFolder[0]);
		return system(command) == 0;
		//		TS_File::EmptyDirectory(m_szTempFolder.c_str());
#endif
	}

#ifdef WIN32
	static bool EmptyDirectory(const char* folder)
	{
		walkDirectoryTree(folder, FileProc_Remove, NULL);
		return RemoveDirectoryA(folder) == TRUE;
	}
#else
	static bool EmptyDirectory(const char* folder)
	{
		char command[256];
		sprintf(command, "rm -rf %s", folder);
		printf("EmptyDirectory command %s\n", command);
		int ret = system(command);
		//		printf("EmptyDirectory finished %s\n",ret == 0?"OK":"Abort");
		return ret == 0;
	}
#endif
#ifdef WIN32
	static bool Exists(const char* folder)
	{
		return _access(folder, 0) == 0;
	}
#else
	static bool Exists(const char* folder)
	{
		return access(folder, 0) == 0;
	}
#endif

#ifdef WIN32
	static bool Move(const char* src, const char* dst)
	{
		return MoveFileA(src, dst) == TRUE;
	}
#else
	static bool Move(const char* src, const char* dst)
	{
		return rename(src, dst) == 0;
	}
#endif

#ifdef WIN32
	static bool Copy(const char* src, const char* dst)
	{
		return ::CopyFileA(src, dst, FALSE) == TRUE;
	}
#else
	static bool Copy(const char* src, const char* dst)
	{
		return execl("/bin/cp", "-p", src, dst) == 0;
	}
#endif
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif