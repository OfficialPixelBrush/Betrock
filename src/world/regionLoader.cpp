#include "regionLoader.h"

uint offset;
uint sector;
std::ifstream f;
const uint maxChunkSize = 16*16*128;

// Get Compression Scheme String
std::string regionLoader::compressionSchemeString(uint cs) {
	switch (cs) {
	case 1:
		return "GZip";
	case 2:
		return "Zlib";
	case 3:
		return "Uncompressed";
	case 4:
		return "LZ4";
	case 127:
		return "Custom";
	}
	return "Unsupported";
}

uint8_t* regionLoader::decompressChunk(uint chunkIndex, size_t length, uint8_t compressionScheme, size_t* nbtLength) {
	// Read Compressed Data from Region File
	char* compressedData = new char[length];
	f.read(reinterpret_cast<char*>(compressedData), length);
	size_t decompressedSize = 100000;
	// Prepare array for Decompressed Data
    uint8_t* decompressedData = (uint8_t*)malloc(decompressedSize);
    if (!decompressedData) {
        fprintf(stderr, "Failed to allocate decompressed buffer\n");
        return NULL;
    }

	// Create Decompressor
	struct libdeflate_decompressor *libd;
	libd = libdeflate_alloc_decompressor();
	if (libd == NULL) {
		cerr << "Could not allocate decompressor!" << endl;
		return NULL;
	}
	// Decompress Data
	int result;
	switch (compressionScheme) {
		//case 1: // TODO: GZip
			// Decompress Data
		//	break;
		case 2: // ZLib
			result = libdeflate_zlib_decompress(libd, compressedData, length, decompressedData, 100000 , &decompressedSize);
			// Decompress Data
			if (result) {
				cerr << "LibDeflate Error #" << to_string(result) << endl;
				return NULL;
			}
			cout << "Deflated Successfully!" << endl;
			break;
		default:
			cerr << "Unknown or Unimplemented compression scheme #" << to_string(compressionScheme) <<"!" << endl;
			return NULL;
	}
	// Deallocate Decompressor
	delete [] compressedData;
	libdeflate_free_decompressor(libd);

	// Return NBT Data
	*nbtLength = decompressedSize;
	return decompressedData;
}

// Returns an array of Chunks
int regionLoader::decodeRegion() {
	// chunk chunks [32*32];
	for (uint chunkIndex = 0; chunkIndex < 2; chunkIndex++) {
		f.seekg(chunkIndex*4,ios::beg);
		// Determine Chunk Position and Size
		offset = intReadFile(f,3)*4096;
		sector = intReadFile(f,1)*4096;
		if (!(offset | sector)) {
			// No Chunk Present
			cerr << "Chunk #" << chunkIndex << " does not exist" << endl;
			continue;
		}
		cout << "Chunk #" << to_string(chunkIndex) << ": " << offset << ", " << sector << "KiB" << endl;
		f.seekg(offset, ios::beg);
		// Determine Chunk metadata
		size_t length = intReadFile(f,4)-1;
		uint8_t compressionScheme = intReadFile(f,1);
		cout << "\t" << length << " Bytes\n\tCompression " << compressionSchemeString(compressionScheme) << endl;

		// Load compressed data
		size_t nbtLength;
		uint8_t* nbtData = decompressChunk(chunkIndex, length, compressionScheme, &nbtLength);

		// Extract Block Data
		nbt nbtLoader;
		TAG_Compound chunkRoot = nbtLoader.loadNbt(nbtData, nbtLength);
		std::cout << chunkRoot.getEntry(0).getIdentifierName() <<": " << chunkRoot.getEntry(0).getName() << std::endl;
	}
	// return chunks;
	return 0;
}

// Get the Region data from the associated regionX and regionZ file
int regionLoader::loadRegion(int x, int z) {
	string regionfile = "world/region/r." + to_string(x) + "." + to_string(z) + ".mcr";
	f.open(regionfile, ios::binary);
	if (!f) {
		cerr << "Region File " << regionfile << " not found!" << endl;
		return 1;
	}
	cout << "Decoding " << regionfile << endl;
	decodeRegion();
	f.close();
	return 0;
}