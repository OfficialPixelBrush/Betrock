#include "world.h"

World::World(std::string pName) {
    this->name = pName;
}

Region* World::getRegion(int x, int z) {
    regionLoader rL(name);
    Chunk* chunks = rL.loadRegion(x,z);

    /*
    // Check if region exists
    if (!regions[x][z]) {
        // If not, attempt to load it
        regionLoader rL(name);
        Chunk* chunks = rL.loadRegion(x,z);
        if (!chunks) {
            std::cout << "Region " << std::to_string(x) << "," << std::to_string(z) << " not found!" << std::endl;
            return nullptr;
        }
        regions[x][z] = new Region(chunks);
    }
    return regions[x][z];*/
    return new Region(chunks);
}

Chunk* World::getChunk(int x, int z) {
    return nullptr;
}

Block* World::getBlock(int x, int y, int z) {
    return nullptr;
}