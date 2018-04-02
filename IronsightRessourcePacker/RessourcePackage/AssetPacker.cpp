#include "stdafx.h"
#include "AssetPacker.h"

#include "RessourcePackage.h"

#include "Crypto\CryptorSeedCBC.h"
#include <zlib\deflate.h>

namespace IronsightRessourcePacker
{
	bool PackAsset(FILE* fp_in, FILE* fp_out, uint32_t* length, RessourcePackageFlags flags)
	{
		uint8_t in[CHUNK];
		uint8_t out[CHUNK];
		uint8_t paddingBuffer[16];
		uint32_t readSize;
		uint32_t finalLength;
		uint32_t startOffset;
		uint32_t endOffset;
		uint32_t decryptedSize;
		uint32_t decompressedSize;
		uint32_t outputSize;
		z_stream_s strm;
		int ret;

		decryptedSize = 0;
		decompressedSize = 0;
		finalLength = 0;
		startOffset = ftell(fp_out);

		if (flags & FLAG_ENCRYPTED)
		{
			finalLength += fwrite(&decryptedSize, 1, sizeof(uint32_t), fp_out);
		}

		if (flags & FLAG_COMPRESSED)
		{
			finalLength += fwrite(&decompressedSize, 1, sizeof(uint32_t), fp_out);
			decryptedSize += sizeof(uint32_t);
		}

		Crypto::CryptorSeedCBC cryptor(SEED_Key, 16, SEED_IV);

		/* allocate inflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;
		ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
		if (ret != Z_OK)
			return false;

		while (true)
		{
			readSize = fread(in, 1, CHUNK, fp_in);

			if (readSize == 0)
				break;

			decompressedSize += readSize;

			if (flags & FLAG_COMPRESSED)
			{
				strm.avail_in = readSize;
				strm.next_in = in;
				// run inflate() on input until output buffer not full
				do
				{
					strm.avail_out = CHUNK;
					strm.next_out = out;
					ret = deflate(&strm, feof(fp_in) ? Z_FINISH : Z_NO_FLUSH);

					assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
					switch (ret)
					{
					case Z_NEED_DICT:
					case Z_DATA_ERROR:
					case Z_MEM_ERROR:
						deflateEnd(&strm);
						return false;
					}

					outputSize = CHUNK - strm.avail_out;
					if (fwrite(out, 1, outputSize, fp_out) != outputSize)
					{
						deflateEnd(&strm);
						return false;
					}
					finalLength += outputSize;
					decryptedSize += outputSize;

				} while (strm.avail_out == 0);
				assert(strm.avail_in == 0);
			}
			else
			{
				finalLength += fwrite(in, 1, readSize, fp_out);
				decryptedSize += readSize;
			}
		}

		deflateEnd(&strm);

		if (flags & FLAG_ENCRYPTED && decryptedSize % Crypto::CryptorSeedCBC::BlockSize)
		{
			memset(paddingBuffer, 0, sizeof(paddingBuffer));

			// Fill up the encryption buffer to a multiple of the blocksize. We will ignore the padding when decrypting.
			finalLength += fwrite(paddingBuffer, 1, Crypto::CryptorSeedCBC::BlockSize - (decryptedSize % Crypto::CryptorSeedCBC::BlockSize), fp_out);
		}

		endOffset = ftell(fp_out);

		fflush(fp_out);
		fseek(fp_out, startOffset, SEEK_SET);

		if (flags & FLAG_ENCRYPTED)
		{
			fwrite(&decryptedSize, 1, sizeof(uint32_t), fp_out);
		}

		if (flags & FLAG_COMPRESSED)
		{
			fwrite(&decompressedSize, 1, sizeof(uint32_t), fp_out);
			fseek(fp_out, -sizeof(uint32_t), SEEK_CUR); // Go back 4 bytes since the decompressed size also gets encrypted
		}

		if (flags & FLAG_ENCRYPTED)
		{
			uint32_t fullEncryptionLength = ((decryptedSize + (Crypto::CryptorSeedCBC::BlockSize - 1)) / Crypto::CryptorSeedCBC::BlockSize) * Crypto::CryptorSeedCBC::BlockSize;
			uint32_t encryptionOffset = 0;

			while (encryptionOffset != fullEncryptionLength)
			{
				readSize = fullEncryptionLength - encryptionOffset;
				if (readSize > CHUNK)
					readSize = CHUNK;

				fread(in, 1, readSize, fp_out);
				fseek(fp_out, -readSize, SEEK_CUR);

				if (!cryptor.Encrypt(in, readSize))
					return false;

				fwrite(in, 1, readSize, fp_out);

				encryptionOffset += readSize;
			}
		}

		fseek(fp_out, endOffset, SEEK_SET);

		assert(endOffset - startOffset == finalLength);

		*length = finalLength;

		return true;
	}
}