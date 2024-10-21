#pragma once
#include "../global.h"
#include "../nbt/nbt.h"
#include "../helper.h"
#include <fstream>
#include "../include/decompress.hpp"
#include <libdeflate.h>
#include "chunk.h"
#include "../compat.h"

class regionLoader {
    std::string path;
    public:
        regionLoader(std::string pPath);
        Chunk* loadRegion(int x, int z);
    private:
        std::string compressionSchemeString(uint cs);
        uint8_t* decompressChunk(uint chunkIndex, size_t length, uint8_t compressionScheme, size_t* nbtLength);
        Chunk* decodeRegion(int x, int z);
};