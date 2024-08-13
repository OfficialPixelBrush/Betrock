// Holds Data for one Region
#include "region.h"

Region::Region(Chunk* chunks) {
    this->chunks = chunks;
}

Chunk* Region::getChunk(int x, int z) {
    return &chunks[x + z*32];
}