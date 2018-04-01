#include "stdafx.h"
#include "AssetPacker.h"

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
}