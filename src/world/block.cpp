#include "block.h"

Block::Block(uint8_t blockType){
    Block::blockType = blockType;

    // Transparent
    if ((blockType == 6) ||
        (blockType >= 8  && blockType <= 11) ||
        (blockType >= 30 && blockType <= 32) ||
        (blockType >= 37 && blockType <= 40) ||
        (blockType == 59) || (blockType == 83) ||
        (blockType == 18) || (blockType == 20))
    {
        Block::transparent = true;
    } else {
        Block::transparent = false;
    }
}