#include "stdafx.h"
#include "RessourcePackage.h"
#include "AssetUnpacker.h"
#include "AssetPacker.h"

namespace IronsightRessourcePacker
{
	uint8_t SEED_Key[]{
		0x88, 0xE3, 0x4F, 0x8F,
		0x08, 0x17, 0x79, 0xF1,
		0xE9, 0xF3, 0x94, 0x37,
		0x0A, 0xD4, 0x05, 0x89,
	};

	uint8_t SEED_IV[]{
		0x26, 0x8D, 0x66, 0xA7,
		0x35, 0xA8, 0x1A, 0x81,
		0x6F, 0xBA, 0xD9, 0xFA,
		0x36, 0x16, 0x25, 0x01
	};

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

	bool RessourcePackage::Save(FILE* fp_out, char* inputBasePath)
	{
		FILE* fp_in;
		char inputFilenameBuffer[MAX_PATH];
		RessourcePackageHeader header;
		uint32_t currentFileOffset;

		header.magic = 'GKPR';
		header.version = 1;
		strncpy(header.packageBasePath, this->basepath, sizeof(RessourcePackageHeader::packageBasePath));
		header.numEntries = this->entries.size();

		currentFileOffset = 0;
		currentFileOffset += fwrite(&header, 1, sizeof(header), fp_out);

		fseek(fp_out, sizeof(RessourcePackageEntry) * header.numEntries, SEEK_CUR);
		currentFileOffset += (sizeof(RessourcePackageEntry) * header.numEntries);

		for (auto& entry : this->entries)
		{
			sprintf_s(inputFilenameBuffer, "%s\\%s", inputBasePath, entry.filename);

			if (fopen_s(&fp_in, inputFilenameBuffer, "rb") == 0)
			{
				uint32_t assetLength;
				entry.offsetInPackage = currentFileOffset;

				PackAsset(fp_in, fp_out, &assetLength, entry.flags);

				entry.sizeInPackage = assetLength;
				currentFileOffset += assetLength;

				fclose(fp_in);
			}
		}

		fseek(fp_out, sizeof(RessourcePackageHeader), SEEK_SET);

		for (auto entry : this->entries)
		{
			fwrite(&entry, 1, sizeof(RessourcePackageEntry), fp_out);
		}

		return false;
	}
}