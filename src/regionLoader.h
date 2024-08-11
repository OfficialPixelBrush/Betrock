#pragma once
#include <iostream>
#include <fstream>
#include <gzip/decompress.hpp>
#include <libdeflate.h>
#include "nbt.h"
using namespace std;

class regionLoader {
    public:
        int loadRegion(int x, int z);
    private:
        uint intread(uint size);
        std::string compressionSchemeString(uint cs);
        uint8_t* decompressChunk(uint chunkIndex, size_t length, uint8_t compressionScheme);
        int decodeRegion();
};