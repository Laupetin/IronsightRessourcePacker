#include "stdafx.h"
#include "Utils.h"

#define BUFFER_FORMAT_SIZE 4096

namespace IronsightUnpacker
{
	char* FormatString(const char* szFormat, ...)
	{
		static int s_textIndex = 0;
		static char s_szText[4][BUFFER_FORMAT_SIZE];

		va_list args;
		va_start(args, szFormat);

		if (++s_textIndex == 4)
			s_textIndex = 0;

		_vsnprintf_s(s_szText[s_textIndex],
			BUFFER_FORMAT_SIZE,
			szFormat,
			args);

		va_end(args);

		s_szText[s_textIndex][BUFFER_FORMAT_SIZE - 1] = '\0';

		return s_szText[s_textIndex];
	}

	void StripFilename(char* pszString)
	{
		for (size_t i = strlen(pszString); i > 0; --i)
		{
			if (pszString[i] == '\\' || pszString[i] == '/')
			{
				pszString[i + 1] = '\0';
				return;
			}
		}
	}

	void StripExtension(char* pszString)
	{
		char* extention = GetExtension(pszString);

		if (extention)
			*extention = '\0';
	}

	char* GetFilename(char* pszString)
	{
		for (size_t i = strlen(pszString); i > 0; --i)
		{
			if (pszString[i] == '\\' || pszString[i] == '/')
			{
				if (pszString[i + 1] == '\0')
					return nullptr;

				return &pszString[i + 1];
			}
		}

		return pszString;
	}

	char* GetExtension(char* pszString)
	{
		char* searchString = pszString;
		char* lastOccurrence = nullptr;

		while (searchString = strchr(searchString, '.'), searchString)
		{
			lastOccurrence = searchString;
			searchString++;
		}

		return lastOccurrence;
	}

	char* GetAppPath()
	{
		static volatile unsigned int inited = 0;
		static bool initDone = false;
		static char szAppPath[MAX_PATH];

		if (InterlockedExchange(&inited, 1) == 0)
		{
			HMODULE hModule = NULL;

			GetModuleHandleExA(
				GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				NULL, &hModule);

			GetModuleFileNameA(hModule, szAppPath, sizeof(szAppPath));

			StripFilename(szAppPath);

			initDone = true;
		}
		while (!initDone);

		return szAppPath;
	}

	void CreateFoldersForPath(char* basepath, char* path)
	{
		char tempPath[MAX_PATH];
		uint32_t basepathLen;
		struct stat sb;
		int folderAt = 0;

		basepathLen = strlen(basepath);
		if (basepathLen)
		{
			sprintf_s(tempPath, "%s\\%s", basepath, path);
		}
		else
		{
			strncpy_s(tempPath, path, MAX_PATH);
		}

		for (int i = strlen(tempPath); i > basepathLen; i--)
		{
			if (tempPath[i] == '/' || tempPath[i] == '\\')
			{
				tempPath[i] = '\0';
				folderAt = i;
				break;
			}
		}

		if (!(stat(tempPath, &sb) == 0 && sb.st_mode & S_IFDIR))
		{
			for (int i = basepathLen; i <= folderAt; i++)
			{
				if (tempPath[i] == '/' || tempPath[i] == '\\')
				{
					tempPath[i] = '\0';
					CreateDirectoryA(tempPath, NULL);
					tempPath[i] = '\\';
				}
				else if (i == folderAt)
				{
					CreateDirectoryA(tempPath, NULL);
				}
			}
		}

		if (tempPath[folderAt] == '\0')
			tempPath[folderAt] = '/';
	}

	size_t FileSize(const char* file)
	{
		struct stat st;

		if (stat(file, &st) >= 0)
		{
			return st.st_size;
		}

		return 0;
	}

	bool FileExists(const char* file)
	{
		struct stat st;

		if (stat(file, &st) >= 0 && !(st.st_mode & S_IFDIR))
		{
			return true;
		}

		return false;
	}

	bool DirectoryExists(const char* dir)
	{
		struct stat info;

		if (stat(dir, &info) != 0)
			return false;
		else if (info.st_mode & S_IFDIR)
			return true;
		else
			return false;
	}
}