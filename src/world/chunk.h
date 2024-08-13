#pragma once
#include "block.h"
#include "../compat.h"

class chunk {
    block data [16*128*16];
    public:
        block* getData() {
            return data;
        }

        void setData(int8_t pData []) {
            for (uint i = 0; i < 16*128*16; i++) {
                block b;
                b.setBlock(pData[i]);
                data[i] = b;
            }
        }
};