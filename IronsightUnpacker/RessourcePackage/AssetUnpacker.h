#pragma once

#ifndef _ASSETUNPACKER_H
#define _ASSETUNPACKER_H

#include "RessourcePackage.h"

namespace IronsightUnpacker
{
	bool UnpackAsset(FILE* fp_in, FILE* fp_out, uint32_t length, RessourcePackageFlags flags);
}

#endif