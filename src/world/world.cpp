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
    // Check Cached Chunk first
    if (cachedChunk && cachedChunk->x == x && cachedChunk->z == z) {
        return cachedChunk;
    }

    // Then check elsewhere
    for (uint i = 0; i < chunks.size(); i++) {
        Chunk* c = chunks[i];
        if (!c) {
            continue;
        }
        if (c->x == x && c->z == z) {
            cachedChunk = c;
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
    if (cachedBlock && cachedBlockX == x && cachedBlockY == y && cachedBlockZ == z) {
        return cachedBlock;
    }
    //std::cout << x << "," << y << "," << z << std::endl;
    Chunk* c = findChunk(floor(float(x)/16.0f),floor(float(z)/16.0f));
    if (c) {
        cachedBlock = c->getBlock(x,y,z);
        cachedBlockX = x;
        cachedBlockY = y;
        cachedBlockZ = z;
        return cachedBlock;
    }
    return nullptr;
}

std::vector<Chunk*> World::getChunksInRadius(int x, int z, int radius) {
    int ix = int(float(x) / 16.0f);
    int iz = int(float(z) / 16.0f);

    std::vector<Chunk*> containedChunks;  // Use raw pointers if you don't want ownership
    std::vector<Chunk*> newChunks;  // Use raw pointers if you don't want ownership

    // Collect new chunks
    for (int cx = -radius; cx < radius; cx++) {
        for (int cz = -radius; cz < radius; cz++) {
            bool newChunk = false;
            if (!findChunk(ix + cx, iz + cz)) {
                newChunk = true;
            }
            Chunk* c = getChunk(ix + cx, iz + cz);
            if (c) {
                containedChunks.push_back(c);
                if (newChunk) {
                    newChunks.push_back(c);
                    // If a chunk neighbors this chunk, add it to the chunk regen queue
                    Chunk* nc = findChunk(ix + cx, iz + cz - 1);
                    Chunk* sc = findChunk(ix + cx, iz + cz + 1);
                    Chunk* ec = findChunk(ix + cx - 1, iz + cz);
                    Chunk* wc = findChunk(ix + cx + 1, iz + cz);
                    if (nc) {
                        newChunks.push_back(nc);
                        //std::cout << "Neighborchunk North" << std::endl;
                    }
                    if (sc) {
                        newChunks.push_back(sc);
                        //std::cout << "Neighborchunk South" << std::endl;
                    }
                    if (ec) {
                        newChunks.push_back(ec);
                        //std::cout << "Neighborchunk East" << std::endl;
                    }
                    if (wc) {
                        newChunks.push_back(wc);
                        //std::cout << "Neighborchunk West" << std::endl;
                    }
                }
            }
        }
    }

    // Clear old chunks if necessary
    chunks.clear();  // Clear the old chunks safely (assuming proper ownership elsewhere)
    
    // If needed, manually reset chunks to ensure no memory leaks
    // Repopulate the old chunks with the new set of pointers
    for (Chunk* chunk : containedChunks) {
        chunks.push_back(chunk);  // Insert back into the main chunk vector
    }
    return newChunks;
}