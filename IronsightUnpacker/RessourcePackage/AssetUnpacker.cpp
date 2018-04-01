#include "stdafx.h"
#include "AssetUnpacker.h"

#include "Crypto\CryptorSeedCBC.h"
#include <zlib\deflate.h>

namespace IronsightUnpacker
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

#define CHUNK 16384

	bool UnpackAsset(FILE* fp_in, FILE* fp_out, uint32_t length, RessourcePackageFlags flags)
	{
		unsigned char in[CHUNK];
		unsigned char out[CHUNK];
		unsigned outputSize;
		z_stream strm;
		int ret;
		uint32_t fileProgress;
		uint32_t compressedProgress;
		uint32_t sizeToRead;
		uint32_t readSize;
		uint32_t decryptedSize;
		uint32_t decompressedSize;

		assert(Crypto::CryptorSeedCBC::IVSize == sizeof(SEED_IV));

		fileProgress = 0;
		decompressedSize = 0;

		if (flags & RessourcePackageFlags::FLAG_ENCRYPTED)
		{
			fileProgress += fread(&decryptedSize, 1, sizeof(decryptedSize), fp_in);
		}
		else
		{
			decryptedSize = length;
		}
		compressedProgress = 0;

		Crypto::CryptorSeedCBC cryptor(SEED_Key, sizeof(SEED_Key), SEED_IV);

		/* allocate inflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;
		ret = inflateInit(&strm);
		if (ret != Z_OK)
			return false;

		do
		{
			sizeToRead = length - fileProgress;
			if (sizeToRead > CHUNK)
				sizeToRead = CHUNK;

			if (sizeToRead == 0)
				return false; // We cannot read anymore but the zstream is not finished yet.. incomplete compression.

			readSize = fread(in, 1, sizeToRead, fp_in);
			fileProgress += readSize;

			if (readSize != sizeToRead)
				return false; // We couldn't read data even though we were supposed to.

			strm.avail_in = decryptedSize - compressedProgress;
			if (strm.avail_in > CHUNK)
				strm.avail_in = CHUNK;

			compressedProgress += strm.avail_in;

			if (flags & RessourcePackageFlags::FLAG_ENCRYPTED && !cryptor.Decrypt(in, readSize))
				return false;

			if (decompressedSize == 0)
			{
				if (flags & RessourcePackageFlags::FLAG_COMPRESSED)
				{
					// The first decompressed 4 byte are the decompressed size of the asset
					decompressedSize = *(uint32_t*)in;
					strm.next_in = in + 4;
					strm.avail_in -= 4;
				}
				else
				{
					decompressedSize = decryptedSize;
				}
			}
			else
			{
				strm.next_in = in;
			}

			if (flags & RessourcePackageFlags::FLAG_COMPRESSED)
			{
				// run inflate() on input until output buffer not full
				do
				{
					strm.avail_out = CHUNK;
					strm.next_out = out;
					ret = inflate(&strm, compressedProgress < decryptedSize ? Z_NO_FLUSH : Z_FINISH);

					assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
					switch (ret)
					{
					case Z_NEED_DICT:
					case Z_DATA_ERROR:
					case Z_MEM_ERROR:
						inflateEnd(&strm);
						return false;
					}

					outputSize = CHUNK - strm.avail_out;
					if (fwrite(out, 1, outputSize, fp_out) != outputSize)
					{
						inflateEnd(&strm);
						return false;
					}
				} while (strm.avail_out == 0);
			}
			else
			{
				if (fwrite(out, 1, strm.avail_in, fp_out) != strm.avail_in)
				{
					inflateEnd(&strm);
					return false;
				}
			}
			
		} while (ret != Z_STREAM_END);

		/* clean up and return */
		inflateEnd(&strm);

		return true;
	}
}