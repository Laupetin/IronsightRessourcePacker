#pragma once

#ifndef _PACKING_H
#define _PACKING_H

#include "RessourcePackage\RessourcePackage.h"

namespace IronsightRessourcePacker
{
	void AddPathToPackage(RessourcePackage* package, char* folderpath, uint32_t pathStripLength);
	bool PackRessourcePackage(char* folderpath);
}

#endif