#include "stdafx.h"
#include "Unpacking.h"

#include "RessourcePackage\RessourcePackage.h"

namespace IronsightRessourcePacker
{
	bool UnpackRessourcePackage(char* filename)
	{
		FILE* fp_in;
		char* inputExtension;
		char* inputFilename;
		char inputFilenameWithoutExtensionBuffer[MAX_PATH];
		char outputPath[MAX_PATH];

		inputExtension = Utils::GetExtension(filename);

		strncpy_s(inputFilenameWithoutExtensionBuffer, filename, MAX_PATH);
		Utils::StripExtension(inputFilenameWithoutExtensionBuffer);
		inputFilename = Utils::GetFilename(inputFilenameWithoutExtensionBuffer);

		if (strcmp(inputExtension, ".wpg") != 0)
		{
			Print("Failed to unpack: Not a wpg file\n");
			return false;
		}

		strncpy_s(outputPath, filename, MAX_PATH);
		Utils::StripFilename(outputPath);
		strcat_s(outputPath, inputFilename);

		if (fopen_s(&fp_in, filename, "rb") == 0)
		{
			RessourcePackage package;

			if (!package.Open(fp_in))
			{
				return false;
			}

			Print("Unpacking RessourcePackage '%s'\n", inputFilename);
			package.DumpAll(outputPath);
		}

		return true;
	}
}