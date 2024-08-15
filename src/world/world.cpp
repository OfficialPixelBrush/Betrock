#include "world.h"

World::World(std::string pName) {
    this->name = pName;
}

Region* World::findRegion(int x, int z) {
    for (auto r : regions) {
        if (r.x == x && r.z == z) {
            return &r;
        }
    }
    return nullptr;
}

Region* World::getRegion(int x, int z) {
    regionLoader rL(name);
    Region* r = findRegion(x,z);
    if (!r) {
        Chunk* chunks = rL.loadRegion(x,z);
        return new Region(chunks,x,z);
    }
    return r;
}

Chunk* World::getChunk(int x, int z) {
    return nullptr;
}

Block* World::getBlock(int x, int y, int z) {
    return nullptr;
}