#include "world.h"

World::World(const std::string& pName) {
    if (pName != "") {
        LoadWorld(pName);
    }
}

void World::LoadWorld(const std::string& pName) {  // Use const reference
    // First assign the new name
    name = pName;  // No need for World:: inside member functions
    
    // Safely delete old RegionLoader
    if (rl != nullptr) {  // Better null check
        delete rl;
        rl = nullptr;     // Good practice to null after delete
    }
    
    // Create new RegionLoader
    rl = new RegionLoader(pName);
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
    Chunk* c = rl->loadRegion(x,z);
    chunks.push_back(c);
    return c;
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
    std::cout << "Getting Chunks" << std::endl;

    // Collect new chunks
    for (int cx = -radius; cx <= radius; cx++) {
        for (int cz = -radius; cz <= radius; cz++) {
            bool newChunk = false;
            Chunk* c = findChunk(ix + cx, iz + cz);
            if (!c) {
                newChunk = true;
                c = loadChunk(ix + cx, iz + cz);
            }

            if (c) {
                containedChunks.push_back(c);
                if (newChunk) {
                    newChunks.push_back(c);
                    // If a chunk neighbors this chunk, add it to the chunk regen queue
                    Chunk* nc = findChunk(ix + cx, iz + cz - 1);
                    Chunk* sc = findChunk(ix + cx, iz + cz + 1);
                    Chunk* ec = findChunk(ix + cx - 1, iz + cz);
                    Chunk* wc = findChunk(ix + cx + 1, iz + cz);
                    
                    // Check and add neighboring chunks only if they are not already in newChunks
                    if (nc && std::find(newChunks.begin(), newChunks.end(), nc) == newChunks.end()) {
                        newChunks.push_back(nc);
                    }
                    if (sc && std::find(newChunks.begin(), newChunks.end(), sc) == newChunks.end()) {
                        newChunks.push_back(sc);
                    }
                    if (ec && std::find(newChunks.begin(), newChunks.end(), ec) == newChunks.end()) {
                        newChunks.push_back(ec);
                    }
                    if (wc && std::find(newChunks.begin(), newChunks.end(), wc) == newChunks.end()) {
                        newChunks.push_back(wc);
                    }
                }
            }
        }
    }

    // Sort newChunks by distance from the point (x, z)
    std::sort(newChunks.begin(), newChunks.end(), [ix, iz](Chunk* a, Chunk* b) {
        // Assuming Chunk has methods getX() and getZ() that return its coordinates
        int ax = a->x;
        int az = a->z;
        int bx = b->x;
        int bz = b->z;

        // Calculate squared distance to avoid the cost of sqrt
        int distA = (ax - ix) * (ax - ix) + (az - iz) * (az - iz);
        int distB = (bx - ix) * (bx - ix) + (bz - iz) * (bz - iz);
        return distA > distB;  // Sort in ascending order (closest first)
    });

    // Clear old chunks if necessary
    chunks.clear();  // Clear the old chunks safely (assuming proper ownership elsewhere)

    // If needed, manually reset chunks to ensure no memory leaks
    // Repopulate the old chunks with the new set of pointers
    for (Chunk* chunk : containedChunks) {
        chunks.push_back(chunk);  // Insert back into the main chunk vector
    }
    std::cout << "Got " << newChunks.size() << " Chunks" << std::endl;

    return newChunks;
}