#pragma once
#include "../global.h"
#include "../compat.h"

class Block {
    public:
        int8_t blockType = 0;

        void setBlock(int8_t pBlockType) {
            this->blockType = pBlockType;
        }

        int8_t getBlock() {
            return blockType;
        }
};