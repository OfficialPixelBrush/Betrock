#pragma once
#include <vector>
#include <algorithm>
#include "chunk.h"
#include "block.h"
#include <cmath>
#include "regionLoader.h"
#include <unordered_set>

class World {
    public:
        RegionLoader* rl;
        std::vector<Chunk*> chunks;
        Chunk* cachedChunk = nullptr;
        Block* cachedBlock = nullptr;
        int cachedBlockX;
        int cachedBlockY;
        int cachedBlockZ;
        std::string name;
        World(std::string pName);

        Chunk* findChunk(int x, int z);
        Chunk* loadChunk(int x, int z);
        Chunk* getChunk(int x, int z);
        Block* getBlock(int x, int y, int z);
        std::vector<Chunk*> getChunksInRadius(int x, int z, int radius);
};