#pragma once
#include "../global.h"
#include "../compat.h"

class Block {
    public:
        uint8_t blockType;
        bool transparent;
        uint8_t skyLightLevel;
        uint8_t lightLevel;
        uint8_t metaData;
        Block(uint8_t blockType = 0, uint8_t skyLightLevel = 15, uint8_t lightLevel = 15, uint8_t metaData = 0);

        void setBlockType(uint8_t pBlockType) {
            this->blockType = pBlockType;
        }

        uint8_t getBlockMetaData() {
            return metaData;
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

        uint8_t getSkyLight() {
            return skyLightLevel;
        }
};