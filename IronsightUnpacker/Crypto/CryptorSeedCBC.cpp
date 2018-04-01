#include "stdafx.h"
#include "CryptorSeedCBC.h"


namespace IronsightUnpacker
{
	namespace Crypto
	{
		void CryptorSeedCBC::Init()
		{
			static bool inited = false;

			if (!inited)
			{
				inited = true;

				if (register_cipher(&kseed_desc) != CRYPT_OK)
				{
					printf("SEED failed: Could not register cipher\n");
					return;
				}
			}
		}

		CryptorSeedCBC::CryptorSeedCBC(uint8_t* key, uint32_t keySize, uint8_t* iv)
		{
			int cipherIndex;

			CryptorSeedCBC::Init();

			cipherIndex = find_cipher(kseed_desc.name);
			if (cipher_is_valid(cipherIndex) != CRYPT_OK)
			{
				printf("SEED failed: Invalid cipher\n");
				return;
			}

			if (cbc_start(cipherIndex, iv, key, keySize, 0, &cbc) != CRYPT_OK)
			{
				printf("SEED failed: cbc_start\n");
				return;
			}
		}

		CryptorSeedCBC::~CryptorSeedCBC()
		{
			cbc_done(&cbc);
		}

		bool CryptorSeedCBC::Decrypt(uint8_t* data, uint32_t dataLen)
		{
			uint32_t maxBlocks;
			uint8_t blockBuffer[BlockSize];

			assert(dataLen % BlockSize == 0);

			if (dataLen % BlockSize)
				return false;

			maxBlocks = dataLen / BlockSize;

			for (uint32_t currentBlock = 0; currentBlock < maxBlocks; currentBlock++)
			{
				if (cbc_decrypt(&data[BlockSize * currentBlock], blockBuffer, BlockSize, &cbc) != CRYPT_OK)
					return false;

				memcpy(&data[BlockSize * currentBlock], blockBuffer, BlockSize);
			}
			
			return true;
		}

		bool CryptorSeedCBC::Encrypt(uint8_t* data, uint32_t dataLen)
		{
			uint32_t maxBlocks;
			uint8_t blockBuffer[BlockSize];

			assert(dataLen % BlockSize == 0);

			if (dataLen % BlockSize)
				return false;

			maxBlocks = dataLen / BlockSize;

			for (uint32_t currentBlock = 0; currentBlock < maxBlocks; currentBlock++)
			{
				if (cbc_encrypt(&data[BlockSize * currentBlock], blockBuffer, BlockSize, &cbc) != CRYPT_OK)
					return false;

				memcpy(&data[BlockSize * currentBlock], blockBuffer, BlockSize);
			}

			return true;
		}
	}
}