#include "stdafx.h"
#include "Packing.h"

namespace IronsightRessourcePacker
{
	std::unordered_map<std::string, uint32_t> flagsForFileExtensions
	{
		std::make_pair(".dds",	1),
		std::make_pair(".phs",	1),
		std::make_pair(".b3d",	1),
		std::make_pair(".cds",	1),
		std::make_pair(".bea",	1),
		std::make_pair(".msh",	1),
		std::make_pair(".lua",	3),
		std::make_pair(".es",	1),
		std::make_pair(".xml",	3),
		std::make_pair(".trg",	3),
		std::make_pair(".txt",	3),
		std::make_pair(".fxo",	1),
		std::make_pair(".ltrl", 1),
		std::make_pair(".strl", 1),
		std::make_pair(".bsn",	1),
		std::make_pair(".fev",	1),
		std::make_pair(".fsb",	1),
		std::make_pair(".nm",	1),
		std::make_pair(".png",	1),
		std::make_pair(".gfx",	1),
		std::make_pair(".gad",	1),
		std::make_pair(".zltl", 1),
		std::make_pair(".bfx",	1),
		std::make_pair(".bpc",	1),
		std::make_pair(".trl",	1),
		std::make_pair(".lhtl", 1),
		std::make_pair(".trn",	1),
		std::make_pair(".jpg",	1),
	};

	static const uint32_t defaultFlags = 1;

	void AddPathToPackage(RessourcePackage* package, char* folderpath, uint32_t pathStripLength)
	{
		char tempPath[MAX_PATH];

		sprintf_s(tempPath, "%s\\*", folderpath);

		WIN32_FIND_DATA data;
		HANDLE hFind;
		if ((hFind = FindFirstFile(tempPath, &data)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					RessourcePackageEntry newEntry;

					sprintf_s(tempPath, "%s\\%s", folderpath, data.cFileName);
					memset(newEntry.filename, 0, sizeof(RessourcePackageEntry::filename));
					strncpy(newEntry.filename, tempPath + pathStripLength + 1, sizeof(RessourcePackageEntry::filename));

					// Check if we have predefined flags for this extension. Otherwise use the default ones.
					auto iterator = flagsForFileExtensions.find(Utils::GetExtension(newEntry.filename));

					if (iterator != flagsForFileExtensions.end())
					{
						newEntry.flags = (RessourcePackageFlags)iterator->second;
					}
					else
					{
						newEntry.flags = (RessourcePackageFlags)defaultFlags;
					}

					// Temporary values. We will change them later when writing.
					newEntry.sizeInPackage = 0;
					newEntry.offsetInPackage = 0;

					package->entries.push_back(newEntry);
				}
				else
				{
					// Filter out the . and .. directories
					if (strcmp(data.cFileName, ".") && strcmp(data.cFileName, ".."))
					{
						sprintf_s(tempPath, "%s\\%s", folderpath, data.cFileName);

						// Do a recursive search through the other directories
						AddPathToPackage(package, tempPath, pathStripLength);
					}
				}

			} while (FindNextFile(hFind, &data) != 0);

			FindClose(hFind);
		}
	}

	bool PackRessourcePackage(char* folderpath)
	{
		FILE* fp_out;
		char* packageName;
		RessourcePackage package;

		char outputPath[MAX_PATH];

		char* firstFile;
		uint32_t commonPathLength;

		packageName = Utils::GetFilename(folderpath);

		AddPathToPackage(&package, folderpath, strlen(folderpath));

		if (package.entries.size() <= 0)
		{
			Print("New package '%s' would not contain any files\n", packageName);
			return false;
		}

		firstFile = package.entries[0].filename;
		commonPathLength = strlen(firstFile);

		for (auto entry : package.entries)
		{
			for (int i = 0; i < commonPathLength; i++)
			{
				if (entry.filename[i] != firstFile[i])
				{
					commonPathLength = i;
					break;
				}
			}
		}

		strncpy_s(package.basepath, firstFile, commonPathLength);

		strncpy_s(outputPath, folderpath, MAX_PATH);
		Utils::StripFilename(outputPath);
		strcat_s(outputPath, packageName);
		strcat_s(outputPath, ".wpg");

		if (fopen_s(&fp_out, outputPath, "wb+") == 0)
		{
			Print("Packing RessourcePackage '%s'\n", packageName);
			package.Save(fp_out, folderpath);

			fclose(fp_out);

			return true;
		}

		return false;
	}
}