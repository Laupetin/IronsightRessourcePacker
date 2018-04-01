#pragma once

#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>

namespace IronsightRessourcePacker
{
	namespace Utils
	{
		extern char* FormatString(const char* szFormat, ...);
		extern void StripFilename(char* pszString);
		extern void StripExtension(char* pszString);
		extern char* GetFilename(char* pszString);
		extern char* GetExtension(char* pszString);
		extern char* GetAppPath();
		extern void CreateFoldersForPath(char* basepath, char* path);
		extern size_t FileSize(const char* file);
		extern bool FileExists(const char* file);
		extern bool DirectoryExists(const char* dir);
	}
}

#endif