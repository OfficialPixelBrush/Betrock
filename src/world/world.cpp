#include "world.h"

World::World(const std::string& pName) {
    if (pName != "") {
        LoadWorld(pName);
    }
}

void World::LoadLevelData() {
    if (this->name == "") {
        return;
    }
    // TODO: Load the seed value from level.dat
    this->seed = 404;
}

void World::LoadWorld(const std::string& pName) {  // Use const reference
    // First assign the new name
    name = pName;  // No need for World:: inside member functions
    LoadLevelData();

    perlin = new Beta173Perlin(this->getSeed());
    
    // Safely delete old RegionLoader
    if (rl != nullptr) {  // Better null check
        delete rl;
        rl = nullptr;     // Good practice to null after delete
    }
    
    // Create new RegionLoader
    rl = new RegionLoader(pName);
}

std::uint64_t World::getSeed() {
    return this->seed;
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
    std::unique_lock<std::shared_mutex> lock(chunk_mutex);
    // Check Cached Chunk first
    if (cachedChunk && cachedChunk->x == x && cachedChunk->z == z) {
        return cachedChunk;
    }
    auto it = chunks.find(std::make_pair(x, z));
    return it != chunks.end() ? it->second : nullptr;
}

Chunk* World::loadChunk(int x, int z) {
    //std::cout << "Load Chunk at " << x/16 << ", " << z/16 << std::endl;
    Chunk* c = rl->loadRegion(x,z);
    addChunk(c);
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
    Chunk* c = findChunk(floor(float(x)/16.0f),floor(float(z)/16.0f));
    if (c) {
        return c->getBlock(x,y,z);
    }
    return nullptr;
}

void World::addChunk(Chunk* chunk) {
    if (chunk) {
        std::unique_lock<std::shared_mutex> lock(chunk_mutex);
        chunks[std::make_pair(chunk->x, chunk->z)] = chunk;
    }
}

void World::removeChunk(int x, int z) {
    std::unique_lock<std::shared_mutex> lock(chunk_mutex);
    auto key = std::make_pair(x, z);
    chunks.erase(key);
}

void World::clearChunks() {
    std::unique_lock<std::shared_mutex> lock(chunk_mutex);
    chunks.clear();
}

size_t World::getNumberOfChunks() {
    return chunks.size();
}

void World::getChunksInRadius(int x, int z, int radius, std::vector<Chunk*>& newChunks, std::mutex& chunkRadiusMutex) {
    int ix = int(float(x) / 16.0f);
    int iz = int(float(z) / 16.0f);

    std::vector<Chunk*> containedChunks;
    std::cout << "Getting Chunks" << std::endl;

    // Collect new chunks within the radius
    for (int ring = 0; ring <= radius; ring++) {
        for (int cx = -ring; cx < ring; cx++) {
            for (int cz = -ring; cz < ring; cz++) {
                bool newChunk = false;
                Chunk* c = findChunk(ix + cx, iz + cz);
                if (!c) {
                    newChunk = true;
                    c = loadChunk(ix + cx, iz + cz);
                }

                if (c) {
                    containedChunks.push_back(c);
                    if (newChunk) {
                        std::unique_lock<std::mutex> lock(chunkRadiusMutex);
                        newChunks.push_back(c);

                        // Check neighboring chunks
                        Chunk* neighbors[] = {findChunk(ix + cx, iz + cz - 1), findChunk(ix + cx, iz + cz + 1),
                                              findChunk(ix + cx - 1, iz + cz), findChunk(ix + cx + 1, iz + cz)};
                        
                        for (Chunk* neighbor : neighbors) {
                            if (neighbor && std::find(newChunks.begin(), newChunks.end(), neighbor) == newChunks.end()) {
                                newChunks.push_back(neighbor);
                            }
                        }
                        lock.unlock();
                    }
                }
            }
        }
    }

    // Delete chunks outside the radius
    std::unique_lock<std::mutex> lock(chunkRadiusMutex);
    for (auto it = newChunks.begin(); it != newChunks.end();) {
        Chunk* chunk = *it;
        int chunkX = chunk->x;
        int chunkZ = chunk->z;

        int distX = abs(chunkX - ix);
        int distZ = abs(chunkZ - iz);

        // Check if the chunk is outside the radius
        if (distX > radius || distZ > radius) {
            delete chunk;
            it = newChunks.erase(it);
        } else {
            ++it;
        }
    }
    lock.unlock();

    std::cout << "Got " << newChunks.size() << " Chunks" << std::endl;
}