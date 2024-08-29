#include "world.h"

World::World(std::string pName) {
    this->name = pName;
}

Region* World::findRegion(int x, int z) {
    for (uint i = 0; i < regions.size(); i++) {
        Region* r = &regions[i];
        if (r->x == x && r->z == z) {
            return r;
        }
    }
    return nullptr;
}

Region* World::getRegion(int x, int z) {
    regionLoader rL(name);
    Region* r = findRegion(x,z);
    if (!r) {
        std::vector<Chunk> chunks = rL.loadRegion(x,z);
        regions.push_back(Region(chunks,x,z));
        return &regions[regions.size()-1];
    }
    return r;
}

Chunk* World::getChunk(int x, int z) {
    return nullptr;
}

Block* World::getBlock(int x, int y, int z) {
    return nullptr;
}