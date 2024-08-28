#pragma once
#include "block.h"
#include "../compat.h"

class Chunk {
    Block data [16*128*16];
    public:
        Block* getAllBlocks() {
            return data;
        }

        Block* getBlock(uint x, uint y, uint z) {
            if (x > 15 || z > 15 || y > 127) {
                return new Block();
            }
            return &data[y + z*128 + (x*128*16)];
        }

        void setData(int8_t pData []) {
            for (uint i = 0; i < 16*128*16; i++) {
                Block b;
                b.setBlockType(pData[i]);
                data[i] = b;
            }
        }
};