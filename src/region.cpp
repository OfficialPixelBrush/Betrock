#include <gzip/decompress.hpp>
#include <libdeflate.h>
#include <iostream>
#include <fstream>
#include <bitset>
using namespace std;
#include "region.h"

uint offset;
uint sector;
ifstream f;
const uint maxChunkSize = 16*16*128;

// Read n-byte integer
uint intread(ifstream& file, uint size) {
	uint integer = 0;
	for (uint i = 0; i < size; i++) {
	char byte = 0;
	file.read(&byte, 1);
	integer = integer << 8;
	integer |= (unsigned char)byte;
	}
	return integer;
}

string csDecode(uint cs) {
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

int decompress(uint chunkIndex, size_t length, uint8_t compressionScheme) {
	// Read Compressed Data from Region File
	char* compressedData = new char[length];
	f.read(reinterpret_cast<char*>(compressedData), length);
	// Prepare array for Decompressed Data
	void* uncompressedData [maxChunkSize];

	// Create Decompressor
	struct libdeflate_decompressor *libd;
	libd = libdeflate_alloc_decompressor();
	if (libd == NULL) {
		cerr << "Could not allocate decompressor!" << endl;
		return 1;
	}
	// Decompress Data
	int result;
	size_t actualOutBytes = 100000;
	switch (compressionScheme) {
		//case 1: // TODO: GZip
			// Decompress Data
		//	break;
		case 2: // ZLib
			result = libdeflate_zlib_decompress(libd, compressedData, length, uncompressedData, 100000 , &actualOutBytes);
			// Decompress Data
			if (result) {
				cerr << "LibDeflate Error #" << to_string(result) << endl;
				return 1;
			}
			cout << "Deflated Successfully!" << endl;
			break;
		default:
			cerr << "Unknown or Unimplemented compression scheme #" << to_string(compressionScheme) <<"!" << endl;
			return 1;
	}
	uint8_t* nbtData = (uint8_t*)uncompressedData;
	/*for (unsigned int i = 0; i < actualOutBytes; i++){
	cout << uD8[i];
	}
	cout << endl;*/
	// Deallocate Decompressor
	delete [] compressedData;
	libdeflate_free_decompressor(libd);

	// Write NBT Data
	ofstream fp;
	string outFile = "chunk_" + to_string(chunkIndex) +"_.nbt";
	fp.open(outFile,ios::out | ios :: binary );
	fp.write((char*)nbtData, actualOutBytes);
	fp.close();
	cout << "Saved " << outFile<< endl;
	return 0;
}

int decodeRegion() {
	for (uint chunkIndex = 0; chunkIndex < 2; chunkIndex++) {
		f.seekg(chunkIndex*4,ios::beg);
		// Determine Chunk Position and Size
		offset = intread(f,3)*4096;
		sector = intread(f,1)*4096;
		if (!(offset | sector)) {
			// No Chunk Present
			cerr << "Chunk #" << chunkIndex << " does not exist" << endl;
			continue;
		}
		cout << "Chunk #" << to_string(chunkIndex) << ": " << offset << ", " << sector << "KiB" << endl;
		f.seekg(offset, ios::beg);
		// Determine Chunk metadata
		size_t length = intread(f,4)-1;
		uint8_t compressionScheme = intread(f,1);
		cout << "\t" << length << " Bytes\n\tCompression " << csDecode(compressionScheme) << endl;

		// Load compressed data
		decompress(chunkIndex, length, compressionScheme);
	}
	return 0;
}

// Get the Region data from the associated regionX and regionZ file
int getRegion(int regionX, int regionZ) {
	string regionfile = "world/region/r." + to_string(regionX) + "." + to_string(regionZ) + ".mcr";
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

int main() {
	getRegion(0,0);
	return 0;
}