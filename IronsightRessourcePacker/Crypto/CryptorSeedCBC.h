#pragma once

#ifndef _CRYPTORSEEDCBC_H
#define _CRYPTORSEEDCBC_H

#include <tomcrypt.h>
#include <stdint.h>

namespace IronsightRessourcePacker
{
	namespace Crypto
	{
		class CryptorSeedCBC
		{
		private:
			symmetric_CBC cbc;
			static void Init();

		public:
			static const uint32_t IVSize = 16;
			static const uint32_t BlockSize = 16;

			CryptorSeedCBC(uint8_t* key, uint32_t keySize, uint8_t* iv);
			~CryptorSeedCBC();

			bool Decrypt(uint8_t* data, uint32_t dataLen);
			bool Encrypt(uint8_t* data, uint32_t dataLen);
		};
	}
}

#endif