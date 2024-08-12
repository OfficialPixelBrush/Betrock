#include "../global.h"

class block {
    public:
        int8_t blockType = 0;

        void setBlock(int8_t pBlockType) {
            this->blockType = pBlockType;
        }

        int8_t getBlock() {
            return blockType;
        }
};