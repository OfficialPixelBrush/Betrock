// Holds Data for one Region
#include "region.h"

Region::Region(std::vector<Chunk> chunks, int x, int z) {
    this->x = x;
    this->z = z;
    this->chunks = chunks;
}

Chunk* Region::getChunk(int x, int z) {
    for (uint i = 0; i < chunks.size(); i++) {
        if (chunks[i].x == x && chunks[i].z == z) {
            return &chunks[i];
        }
    }
    return nullptr;
}