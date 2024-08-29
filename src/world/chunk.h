#pragma once
#include "block.h"
#include "../compat.h"

class Chunk {
    Block blocks [16*128*16];
    public:
        int x,z;
        Chunk(int x, int z) {
            this->x = x;
            this->z = z;
        }

        Block* getAllBlocks() {
            return blocks;
        }

        Block* getBlock(uint x, uint y, uint z) {
            if (x > 15 || z > 15 || y > 127) {
                return new Block();
            }
            return &blocks[y + z*128 + (x*128*16)];
        }

        void setData(int8_t pBlockData [], int8_t pBlockLightData []) {
            for (uint i = 0; i < 16*128*16; i++) {
                blocks[i] = Block(pBlockData[i],pBlockLightData[i]);
            }
        }
};