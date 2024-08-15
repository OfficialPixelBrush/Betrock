// Holds Data for one Region
#include "region.h"

Region::Region(Chunk* chunks, int x, int z) {
    this->x = x;
    this->z = z;
    this->chunks = chunks;
}

Chunk* Region::getChunk(int x, int z) {
    return &chunks[x + z*32];
}