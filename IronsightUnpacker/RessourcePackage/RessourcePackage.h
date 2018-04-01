#pragma once

#ifndef _RESSOURCEPACKAGE_H
#define _RESSOURCEPACKAGE_H

#include <stdint.h>

namespace IronsightUnpacker
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
		std::vector<RessourcePackageEntry> entries;
		char basepath[128];
		FILE* fp;

	public:
		bool Open(FILE* fp);
		bool DumpAll(char* outputBasePath);
	};
}

#endif