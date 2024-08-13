#pragma once
#include <vector> 
#include "region.h"
#include "chunk.h"
#include "block.h"

class World {
    public:
        //vector<Region> v;
        std::string name;
        std::vector<std::vector<Region*>> regions;
        World(std::string pName);

        Region* getRegion(int x, int z);
        Chunk* getChunk(int x, int z);
        Block* getBlock(int x, int y, int z);
};