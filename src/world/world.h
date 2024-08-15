#pragma once
#include <vector> 
#include "region.h"
#include "chunk.h"
#include "block.h"

class World {
    public:
        std::vector<Region> regions;
        std::string name;
        World(std::string pName);
        
        Region* findRegion(int x, int z);
        Region* getRegion(int x, int z);
        Chunk* getChunk(int x, int z);
        Block* getBlock(int x, int y, int z);
};