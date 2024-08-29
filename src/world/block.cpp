#include "block.h"
uint8_t transparentBlocks [] = {6,8,9,10,11,18,20,27,28,30,31,32,37,38,39,40,44,50,51,52,53,55,59,60,63,64,65,66,67,68,69,70,71,72,75,76,77,79,81,83,85,90,92,93,94,96};

Block::Block(uint8_t blockType, uint8_t lightLevel){
    Block::blockType = blockType;
    Block::lightLevel = lightLevel;

    // Transparent
    Block::transparent = false;
    for (uint i = 0; i < sizeof(transparentBlocks); i++) {
        if (blockType == transparentBlocks[i]) {
            Block::transparent = true;
            break;
        }
    }
}