#pragma once
#include "../global.h"
#include "../compat.h"

class Block {
    public:
        int8_t blockType = 0;
        bool transparent = false;
        // TODO: implement so we can properly give each block it's own texture
        //glm::vec2 textureUV();

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