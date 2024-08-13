#pragma once
#include "chunk.h"
#include "regionLoader.h"
#include "../compat.h"

class Region {
    public:
        Chunk* chunks;
        Region(Chunk* chunks);
        Chunk* getChunk(int x, int z);
};