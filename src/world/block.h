#pragma once
#include "../global.h"
#include "../compat.h"

class Block {
    public:
        int8_t blockType;
        bool transparent;
        Block(uint8_t blockType = 0);

        void setBlockType(int8_t pBlockType) {
            this->blockType = pBlockType;
        }

        int8_t getBlockType() {
            return blockType;
        };

        bool getTransparent() {
            return transparent;
        };
};