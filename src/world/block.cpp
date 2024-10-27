#include <unordered_set>
#include <cstdint>
#include "block.h"

// Initialize transparent and light source blocks as unordered sets for faster lookup
Block::Block(uint8_t blockType, uint8_t skyLightLevel, uint8_t lightLevel, uint8_t metaData)
    : blockType(blockType), skyLightLevel(skyLightLevel), lightLevel(lightLevel), metaData(metaData) {
}
