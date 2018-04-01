#pragma once

#ifndef _RESSOURCEPACKAGE_H
#define _RESSOURCEPACKAGE_H

#include <stdint.h>

namespace IronsightRessourcePacker
{
	enum RessourcePackageFlags : uint32_t
	{
		FLAG_COMPRESSED = 1 << 0,
		FLAG_ENCRYPTED = 1 << 1,
	};

	struct RessourcePackageHeader
	{
		uint32_t magic;
		uint32_t version;
		char packageBasePath[128];
		uint32_t numEntries;
	};

	struct RessourcePackageEntry
	{
		char filename[128];
		uint32_t offsetInPackage;
		uint32_t sizeInPackage;
		RessourcePackageFlags flags;
	};

	class RessourcePackage
	{
	private:
		FILE* fp;

	public:
		char basepath[128];
		std::vector<RessourcePackageEntry> entries;

		bool Open(FILE* fp);
		bool DumpAll(char* outputBasePath);
		bool Save(FILE* fp, char* inputBasePath);
	};
}

#endif