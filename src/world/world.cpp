#include "world.h"

World::World(std::string pName) {
    this->name = pName;
}
/*
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
}*/

Chunk* World::findChunk(int x, int z) {
    for (uint i = 0; i < chunks.size(); i++) {
        Chunk* c = chunks[i];
        if (c->x == x && c->z == z) {
            return c;
        }
    }
    return nullptr;
}

Chunk* World::loadChunk(int x, int z) {
    //std::cout << "Load Chunk at " << x/16 << ", " << z/16 << std::endl;
    regionLoader rL(name);
    chunks.push_back(rL.loadRegion(x,z));
    return findChunk(x,z);
}

Chunk* World::getChunk(int x, int z) {
    Chunk* c = findChunk(x,z);
    if (c) {
        return c;
    }
    return loadChunk(x,z);
}

Block* World::getBlock(int x, int y, int z) {
    //std::cout << x << "," << y << "," << z << std::endl;
    Chunk* c = findChunk(floor(float(x)/16.0f),floor(float(z)/16.0f));
    if (c) {
        return c->getBlock(x,y,z);
    }
    return new Block();
}

void World::getChunksInRadius(int x, int z, int radius) {
    int ix = int(float(x)/16.0f);
    int iz = int(float(z)/16.0f);
    std::vector<Chunk*> containedChunks;
    for (int cx = radius*-1; cx < radius; cx++) {
        for (int cz = radius*-1; cz < radius; cz++) {
            Chunk* c = getChunk(ix+cx,iz+cz);
            if (c) {
                containedChunks.push_back(c);
            }
        }
    }
    chunks = containedChunks;
}