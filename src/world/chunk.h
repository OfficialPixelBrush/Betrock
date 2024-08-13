#pragma once
#include "block.h"
#include "../compat.h"

class Chunk {
    Block data [16*128*16];
    public:
        Block* getData() {
            return data;
        }

        void setData(int8_t pData []) {
            for (uint i = 0; i < 16*128*16; i++) {
                Block b;
                b.setBlock(pData[i]);
                data[i] = b;
            }
        }
};