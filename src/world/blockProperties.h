// block_properties.h
#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
#include <string>

// Maximum block types supported (96 blocks in Minecraft as of Beta 1.7.3)
constexpr size_t MAX_BLOCK_TYPES = 97;

// Global lookup tables
extern std::array<bool, MAX_BLOCK_TYPES> cubeLUT;
extern std::array<bool, MAX_BLOCK_TYPES> transparentBlocksLUT;
extern std::array<bool, MAX_BLOCK_TYPES> lightSourceBlocksLUT;
extern std::array<bool, MAX_BLOCK_TYPES> partialBlocksLUT;
extern std::array<bool, MAX_BLOCK_TYPES> nonSolidBlocksLUT;
extern std::array<bool, MAX_BLOCK_TYPES> fluidBlocksLUT;

// Function to initialize the lookup tables (call once at program startup)
void initializeBlockLUTs();
bool isCube(uint8_t blockType);
bool isTransparent(uint8_t blockType);
bool isPartialBlock(uint8_t blockType);
bool isLightSource(uint8_t blockType);
bool isFluid(uint8_t blockType);
bool isNonSolid(uint8_t blockType);
std::string getBlockName(uint8_t blockType);