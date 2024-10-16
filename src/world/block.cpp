#include <unordered_set>
#include <cstdint>
#include "block.h"

// Initialize transparent and light source blocks as unordered sets for faster lookup
std::unordered_set<uint8_t> transparentBlocks = {0, 6, 8, 9, 10, 11, 18, 20, 27, 28, 30, 31, 32, 37, 38, 39, 40, 44, 50, 51, 52, 53, 55, 59, 60, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 75, 76, 77, 78, 79, 81, 83, 85, 90, 92, 93, 94, 96};
std::unordered_set<uint8_t> lightSourceBlocks = {10, 11, 50, 51, 62, 76, 89, 91, 124};
std::unordered_set<uint8_t> nonFullBlocks = {0,26,27,28,30,31,37,38,39,40,44,50,51,53,55,59,60,63,64,65,66,67,68,69,70,71,72,75,76,77,81,83,85,88,90,92,93,94,96,101,102,104,105,106,107,108,109,111,113,114,115,116,117,126};
std::unordered_set<uint8_t> nonSolidBlocks = {0,6,8,9,10,11,27,28,30,31,32,37,38,39,40,50,51,55,59,63,64,65,66,68,69,72,75,76,77,78,83,93,94};

Block::Block(uint8_t blockType, uint8_t skyLightLevel, uint8_t lightLevel, uint8_t metaData)
    : blockType(blockType), skyLightLevel(skyLightLevel), lightLevel(lightLevel), metaData(metaData) {
    
    // Transparent
    Block::transparent = (transparentBlocks.find(blockType) != transparentBlocks.end());
    Block::lightSource = (lightSourceBlocks.find(blockType) != lightSourceBlocks.end());
    Block::partialBlock = (nonFullBlocks.find(blockType) != lightSourceBlocks.end());
    Block::nonSolidBlock = (nonSolidBlocks.find(blockType) != nonSolidBlocks.end());
}
