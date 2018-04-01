#include "stdafx.h"
#include "RessourcePackage.h"
#include "AssetUnpacker.h"

namespace IronsightRessourcePacker
{
	bool RessourcePackage::Open(FILE* fp)
	{
		RessourcePackageHeader header;
		RessourcePackageEntry newEntry;
		this->fp = fp;

		if (fread(&header, 1, sizeof(header), fp) != sizeof(header))
		{
			Print("Could not read header\n");
			return false;
		}

		if (header.magic != 'GKPR')
		{
			Print("Invalid header\n");
			return false;
		}
		
		if (header.version > 1)
		{
			Print("Ressource package is newer than the last version supported (%u > %u)\n", header.version, 1);
			return false;
		}

		if (header.numEntries <= 0)
		{
			Print("Ressource package does not contain any entries\n");
			return false;
		}

		strncpy_s(this->basepath, header.packageBasePath, sizeof(RessourcePackage::basepath));

		for (int i = 0; i < header.numEntries; i++)
		{
			if (!fread(&newEntry, 1, sizeof(RessourcePackageEntry), fp))
			{
				Print("Could not read entry %u\n", i);
				return false;
			}

			this->entries.push_back(newEntry);
		}

		return true;
	}

	bool RessourcePackage::DumpAll(char* outputBasePath)
	{
		char outputFilePath[MAX_PATH];
		FILE* fp_out;
		int currentEntry = 0;
		int entryCount = this->entries.size();

		for (auto entry : this->entries)
		{
			std::cout << (currentEntry) << " / " << (entryCount) << '\r';

			fseek(this->fp, entry.offsetInPackage, SEEK_SET);

			sprintf_s(outputFilePath, "%s\\%s", outputBasePath, entry.filename);

			Utils::CreateFoldersForPath(outputBasePath, entry.filename);

			if (fopen_s(&fp_out, outputFilePath, "wb") == 0)
			{
				if (!UnpackAsset(this->fp, fp_out, entry.sizeInPackage, entry.flags))
				{
					Print("Failed to unpack asset '%s'\n", entry.filename);
				}

				fclose(fp_out);
			}
			else
			{
				Print("Failed to open outputfile for asset '%s'\n", entry.filename);
			}

			currentEntry++;
		}
		Print("%u / %u\n", entryCount, entryCount);

		return true;
	}
}