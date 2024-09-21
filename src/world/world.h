#pragma once
#include <vector>
#include "chunk.h"
#include "block.h"
#include <cmath>
#include "regionLoader.h"
#include <unordered_set>

class World {
    public:
        std::vector<Chunk*> chunks;
        std::string name;
        World(std::string pName);

        Chunk* findChunk(int x, int z);
        Chunk* loadChunk(int x, int z);
        Chunk* getChunk(int x, int z);
        Block* getBlock(int x, int y, int z);
        std::vector<Chunk*> getChunksInRadius(int x, int z, int radius);
};