#pragma once

#ifndef _ASSETPACKER_H
#define _ASSETPACKER_H

#include "RessourcePackage.h"

namespace IronsightRessourcePacker
{
	bool PackAsset(FILE* fp_in, FILE* fp_out, uint32_t* length, RessourcePackageFlags flags);
}

#endif