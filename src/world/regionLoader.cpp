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
chunk* regionLoader::decodeRegion() {
	chunk* chunks = new chunk[32*32];
	for (uint chunkIndex = 0; chunkIndex < 1; chunkIndex++) {
		chunk currentChunk;
		f.seekg(chunkIndex*4,ios::beg);
		// Determine Chunk Position and Size
		offset = intReadFile(f,3)*4096;
		sector = intReadFile(f,1)*4096;
		if (!(offset | sector)) {
			// No Chunk Present
			// cerr << "Chunk #" << chunkIndex << " does not exist" << endl;
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
		TAG_Compound* chunkRoot = nbtLoader.loadNbt(nbtData, nbtLength);
		nbtTag* entry = chunkRoot->getData(0);
		auto* chunkLevel = dynamic_cast<TAG_Compound*>(entry);
		if (!chunkLevel) {
			std::cerr << "The entry is not of type TAG_Compound!" << std::endl;
			return NULL;
		}
		int8_t* blockData;
		for (uint i = 0; i < chunkLevel->getSizeOfData(); i++) {
			if (chunkLevel->getData(i)->getName() == "Blocks") {
				std::cout << "Block data found!" << std::endl;
				auto* blockArray = dynamic_cast<TAG_Byte_Array*>( chunkLevel->getData(i) );
				blockData = blockArray->getData();
				break;				
			}
		}
		if (!blockData) {
			std:cerr << "No block data found!" << std::endl;
			return NULL;
		}
		currentChunk.setData(blockData);
		chunks[chunkIndex] = currentChunk;
	}
	return chunks;
}

// Get the Region data from the associated regionX and regionZ file
chunk* regionLoader::loadRegion(int x, int z) {
	string regionfile = "world/region/r." + to_string(x) + "." + to_string(z) + ".mcr";
	f.open(regionfile, ios::binary);
	if (!f) {
		cerr << "Region File " << regionfile << " not found!" << endl;
		return NULL;
	}
	cout << "Decoding " << regionfile << endl;
	chunk* chunks = decodeRegion();
	f.close();
	return chunks;
}