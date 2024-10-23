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
	// Issue was caused by too small decompressed size guess
	size_t decompressedSize = 1000000;
	
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
Chunk* regionLoader::decodeRegion(int chunkX, int chunkZ) {
	//for (uint chunkIndex = 0; chunkIndex < 32*32; chunkIndex++) {
	uint chunkIndex = (chunkX&31) + (chunkZ&31)*32;
	f.seekg(chunkIndex*4,std::ios::beg);
	// Determine Chunk Position and Size
	offset = intReadFile(f,3)*4096;
	sector = intReadFile(f,1)*4096;
	if (!(offset | sector)) {
		// No Chunk Present
		//std::cerr << "Chunk #" << chunkIndex << " does not exist" << std::endl;
		//continue;
		return nullptr;
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
	auto* chunkLevel = dynamic_cast<TAG_Compound*>(nbtLoader.loadNbt(nbtData, nbtLength)->getData(0));
	if (!chunkLevel) {
		std::cerr << "The entry is not of type TAG_Compound!" << std::endl;
		//continue;
		return nullptr;
	}

	// Get Block Data
	int8_t* blockData;
	int8_t* blockSkyLightData;
	int8_t* blockLightData;
	int8_t* blockMetaData;
	bool foundBlockData = false;
	bool foundSkyLightData = false;
	bool foundLightData = false;
	bool foundMetaData = false;
	for (uint i = 0; i < chunkLevel->getSizeOfData(); i++) {
		// Get Block ID
		if (chunkLevel->getData(i)->getName() == "Blocks") {
			auto* blockArray = dynamic_cast<TAG_Byte_Array*>( chunkLevel->getData(i) );
			blockData = blockArray->getData();
			foundBlockData = true;
		}
		// Get Block Sky Light
		if (chunkLevel->getData(i)->getName() == "SkyLight") {
			auto* blockSkyLightArray = dynamic_cast<TAG_Byte_Array*>( chunkLevel->getData(i) );
			blockSkyLightData = blockSkyLightArray->getData();
			foundSkyLightData = true;		
		}
		// Get Block Light
		if (chunkLevel->getData(i)->getName() == "BlockLight") {
			auto* blockLightArray = dynamic_cast<TAG_Byte_Array*>( chunkLevel->getData(i) );
			blockLightData = blockLightArray->getData();
			foundLightData = true;		
		}
		// Get Block Metadata
		if (chunkLevel->getData(i)->getName() == "Data") {
			auto* blockMetaDataArray = dynamic_cast<TAG_Byte_Array*>( chunkLevel->getData(i) );
			blockMetaData = blockMetaDataArray->getData();
			foundMetaData = true;
		}
		// If all has been found, gtfo
		if (foundBlockData && foundSkyLightData && foundLightData && foundMetaData) {
			break;
		}
	}
	if (!blockData) {
		std::cerr << "No block data found!" << std::endl;
		return nullptr;
	}
	if (!blockSkyLightData) {
		std::cerr << "No sky light data found!" << std::endl;
		return nullptr;
	}
	if (!blockLightData) {
		std::cerr << "No block light data found!" << std::endl;
		return nullptr;
	}
	if (!blockMetaData) {
		std::cerr << "No block metadata found!" << std::endl;
		return nullptr;
	}
	return new Chunk(chunkX,chunkZ,blockData,blockSkyLightData,blockLightData,blockMetaData);
}

regionLoader::regionLoader(std::string pPath) {
	this->path = pPath;
}

// Get the Region data from the associated regionX and regionZ file
Chunk* regionLoader::loadRegion(int chunkX, int chunkZ) {
    int regionX = (int) std::floor(chunkX / 32.0f);
    int regionZ = (int) std::floor(chunkZ / 32.0f);
	std::string regionfile = path + "region/r." + std::to_string(regionX) + "." + std::to_string(regionZ) + ".mcr";
	f.open(regionfile, std::ios::binary);
	if (!f.is_open()) {
		std::cerr << "Region File " << regionfile << " not found!" << std::endl;
		return nullptr;
	}
	//std::cout << "Decoding " << regionfile << std::endl;
	Chunk* chunk = decodeRegion(chunkX,chunkZ);
	f.close();
	return chunk;
}