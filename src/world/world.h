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
        RegionLoader* rl = nullptr;
        std::vector<Chunk*> chunks;
        Chunk* cachedChunk = nullptr;
        Block* cachedBlock = nullptr;
        int cachedBlockX;
        int cachedBlockY;
        int cachedBlockZ;
        std::string name;
        World(const std::string& pName = "");
        void LoadWorld(const std::string& pName);
        // Also add a destructor if you haven't already
        ~World() {
            if (rl != nullptr) {
                delete rl;
                rl = nullptr;
            }
        }

        Chunk* findChunk(int x, int z);
        Chunk* loadChunk(int x, int z);
        Chunk* getChunk(int x, int z);
        Block* getBlock(int x, int y, int z);
        std::vector<Chunk*> getChunksInRadius(int x, int z, int radius);
};