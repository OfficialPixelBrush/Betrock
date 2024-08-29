#include "regionLoader.h"

uint offset;
uint sector;
std::ifstream f;

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
		std::cerr << "Could not allocate decompressor!" << std::endl;
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
				std::cerr << "LibDeflate Error #" << std::to_string(result) << std::endl;
				return NULL;
			}
			//std::cout << "Deflated Successfully!" << std::endl;
			break;
		default:
			std::cerr << "Unknown or Unimplemented compression scheme #" << std::to_string(compressionScheme) <<"!" << std::endl;
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
std::vector<Chunk> regionLoader::decodeRegion() {
	std::vector<Chunk> chunks;
	for (uint chunkIndex = 0; chunkIndex < 32*32; chunkIndex++) {
		Chunk currentChunk(chunkIndex%32, chunkIndex/32);
		f.seekg(chunkIndex*4,std::ios::beg);
		// Determine Chunk Position and Size
		offset = intReadFile(f,3)*4096;
		sector = intReadFile(f,1)*4096;
		if (!(offset | sector)) {
			// No Chunk Present
			// cerr << "Chunk #" << chunkIndex << " does not exist" << endl;
			continue;
		}
		//std::cout << "Chunk #" << std::to_string(chunkIndex) << ": " << offset << ", " << sector << "KiB" << std::endl;
		f.seekg(offset, std::ios::beg);
		// Determine Chunk metadata
		size_t length = intReadFile(f,4)-1;
		uint8_t compressionScheme = intReadFile(f,1);
		//std::cout << "\t" << length << " Bytes\n\tCompression " << compressionSchemeString(compressionScheme) << std::endl;

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
			continue;
		}

		// Get Block Data
		int8_t* blockData;
		for (uint i = 0; i < chunkLevel->getSizeOfData(); i++) {
			if (chunkLevel->getData(i)->getName() == "Blocks") {
				//std::cout << "Block data found!" << std::endl;
				auto* blockArray = dynamic_cast<TAG_Byte_Array*>( chunkLevel->getData(i) );
				blockData = blockArray->getData();
				break;				
			}
		}

		// Get Blocklight Data
		int8_t* blockLightData;
		for (uint i = 0; i < chunkLevel->getSizeOfData(); i++) {
			if (chunkLevel->getData(i)->getName() == "BlockLight") {
				//std::cout << "Block light data found!" << std::endl;
				auto* blockLightArray = dynamic_cast<TAG_Byte_Array*>( chunkLevel->getData(i) );
				blockLightData = blockLightArray->getData();
				break;				
			}
		}
		if (!blockData) {
			std::cerr << "No block data found!" << std::endl;
			continue;
		}
		if (!blockLightData) {
			std::cerr << "No block light data found!" << std::endl;
			continue;
		}
		currentChunk.setData(blockData,blockLightData);
		chunks.push_back(currentChunk);
	}
	return chunks;
}

regionLoader::regionLoader(std::string pPath) {
	this->path = pPath;
}

// Get the Region data from the associated regionX and regionZ file
std::vector<Chunk> regionLoader::loadRegion(int x, int z) {
	std::string regionfile = path + "/region/r." + std::to_string(x) + "." + std::to_string(z) + ".mcr";
	f.open(regionfile, std::ios::binary);
	if (!f) {
		std::cerr << "Region File " << regionfile << " not found!" << std::endl;
	}
	std::cout << "Decoding " << regionfile << std::endl;
	std::vector<Chunk> chunks = decodeRegion();
	f.close();
	return chunks;
}