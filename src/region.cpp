#include <gzip/decompress.hpp>
#include <libdeflate.h>
#include <iostream>
#include <fstream>
#include <bitset>
using namespace std;
#include "region.h"

uint offset;
uint sector;
const uint maxChunkSize = 16*16*128;

// Read n-byte integer
uint intread(ifstream & file, uint size) {
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

int decode(int regionX, int regionZ) {
    string regionfile = "../world/region/r." + to_string(regionX) + "." + to_string(regionZ) + ".mcr";
    ifstream f;
    f.open(regionfile, ios::binary);
    if (!f) {
        cerr << "Region File " << regionfile << " not found!" << endl;
        return 1;
    } else {
        cout << "Decoding " << regionfile << endl;
        for (uint c = 0; c < 2; c++) {
            f.seekg(c*4,ios::beg);
            // Determine Chunk Position and Size
            offset = intread(f,3)*4096;
            sector = intread(f,1)*4096;
            if (offset | sector) {
                cout << "Chunk #" << to_string(c) << ": " << to_string(offset) << ", " << to_string(sector) << "KiB" << endl;
                f.seekg(offset, ios::beg);
                // Determine Chunk metadata
                size_t length = intread(f,4)-1;
                uint8_t compressionScheme = intread(f,1);
                cout << "\t" << to_string(length) << " Bytes\n\tCompression " << csDecode(compressionScheme) << endl;

                // Load compressed data
                cout << "Pos: " << to_string(f.tellg()) << endl;
                char* compressedData = new char[length];
                f.read(reinterpret_cast<char*>(compressedData), length);

                void* uncompressedData [maxChunkSize];

                // Create Decompressor
                struct libdeflate_decompressor *libd;
                libd = libdeflate_alloc_decompressor();
                if (libd == NULL) {
                    cerr << "Could not allocate decompressor!" << endl;
                    return 1;
                }
                int result;
                size_t actualOutBytes;
                actualOutBytes = 100000;
                switch (compressionScheme) {
                    case 1: // GZip
                        // Decompress Data
                        break;
                    case 2: // ZLib
                        result = libdeflate_zlib_decompress(libd, compressedData, length, uncompressedData, 100000 , &actualOutBytes);
                        //uncompressedDataString = libdeflate_deflate_decompress(libd, maxChunkSize);
                        //cout << uncompressedDataString;
                        // Decompress Data
                        if (result == LIBDEFLATE_SUCCESS) {
                            cout << "Deflated Successfully!\n";
                        } else {
                            cerr << "Deflate Error #" << to_string(result) << endl;
                            return 1;
                        }
                        break;
                    default:
                        cerr << "Unknown compression scheme #" << to_string(compressionScheme) <<"!" << endl;
                        return 1;
                }
                uint8_t* myUint8Pointer = (uint8_t*)uncompressedData;
                for (unsigned int i = 0; i < 50; i++){
                    cout << myUint8Pointer[i];
                }
                delete [] compressedData;
                libdeflate_free_decompressor(libd);
            }
        }
        f.close();
        return 0;
    }
}