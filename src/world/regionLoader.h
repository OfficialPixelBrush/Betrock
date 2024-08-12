#pragma once
#include "../global.h"
#include "../nbt/nbt.h"
#include "../helper.h"
#include <fstream>
#include <gzip/decompress.hpp>
#include <libdeflate.h>
#include "chunk.h"
using namespace std;

class regionLoader {
    public:
        chunk* loadRegion(int x, int z);
    private:
        std::string compressionSchemeString(uint cs);
        uint8_t* decompressChunk(uint chunkIndex, size_t length, uint8_t compressionScheme, size_t* nbtLength);
        chunk* decodeRegion();
};