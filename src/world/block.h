#pragma once
#include "../global.h"
#include "../compat.h"

class Block {
    public:
        uint8_t blockType;
        bool transparent;
        uint8_t lightLevel;
        Block(uint8_t blockType = 0, uint8_t lightLevel = 15);

        void setBlockType(uint8_t pBlockType) {
            this->blockType = pBlockType;
        }

        uint8_t getBlockType() {
            return blockType;
        };

        bool getTransparent() {
            return transparent;
        };

        uint8_t getBlockLight() {
            return lightLevel;
        }
};