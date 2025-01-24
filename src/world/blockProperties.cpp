// block_properties.cpp
#include "blockProperties.h"

// Define the global lookup tables
std::array<bool, MAX_BLOCK_TYPES> transparentBlocksLUT = { false };
std::array<bool, MAX_BLOCK_TYPES> lightSourceBlocksLUT = { false };
std::array<bool, MAX_BLOCK_TYPES> partialBlocksLUT = { false };
std::array<bool, MAX_BLOCK_TYPES> nonSolidBlocksLUT = { false };
std::array<bool, MAX_BLOCK_TYPES> fluidBlocksLUT = { false };
std::array<bool, MAX_BLOCK_TYPES> billboardBlocksLUT = { false };

// Initialize the lookup tables
void initializeBlockLUTs() {
    // Transparent blocks
    for (uint8_t blockType : {0, 6, 8, 9, 10, 11, 18, 20, 27, 28, 30, 31, 32, 37, 38, 39, 40, 51, 52, 55, 59, 65, 66, 79, 81, 83, 90, 96}) {
        transparentBlocksLUT[blockType] = true;
    }

    // Light source blocks
    for (uint8_t blockType : {10, 11, 50, 51, 62, 76, 89, 91}) {
        lightSourceBlocksLUT[blockType] = true;
    }

    // Partial blocks
    // Blocks that do not take up a full Block
    for (uint8_t blockType : {6,8,9,10,11,26,27,28,30,31,32,34,37,38,44,50,51,53,55,59,60,63,64,65,66,67,68,69,70,71,72,75,76,77,78,81,83,85,88,90,92,93,94,96}) {
        partialBlocksLUT[blockType] = true;
    }

    // Non-solid blocks
    for (uint8_t blockType : {0, 6, 8, 9, 10, 11, 27, 28, 30, 31, 32, 37, 38, 39, 40, 50, 51, 55, 59, 63, 64, 65, 66, 68, 69, 72, 75, 76, 77, 78, 83, 93, 94}) {
        nonSolidBlocksLUT[blockType] = true;
    }

    // Fluid blocks
    for (uint8_t blockType : {8,9,10,11}) {
        fluidBlocksLUT[blockType] = true;
    }

    // Billboarded blocks
    for (uint8_t blockType : {6,29,30,31,32,37,38,39,40,83}) {
        billboardBlocksLUT[blockType] = true;
    }
}

bool isTransparent(uint8_t blockType) {
    return transparentBlocksLUT[blockType];
};

bool isPartialBlock(uint8_t blockType) {
    return partialBlocksLUT[blockType];
};

bool isLightSource(uint8_t blockType) {
    return lightSourceBlocksLUT[blockType];
};

bool isFluid(uint8_t blockType) {
    return fluidBlocksLUT[blockType];
}

bool isBillboard(uint8_t blockType) {
    return billboardBlocksLUT[blockType];
}

bool isNonSolid(uint8_t blockType) {
    return nonSolidBlocksLUT[blockType];
}

std::string getBlockName(uint8_t blockType) {
    switch(blockType) {
        case 0:
            return "Air";
        case 1:
            return "Stone";
        case 2:
            return "Grass";
        case 3:
            return "Dirt";
        case 4:
            return "Cobblestone";
        case 5:
            return "Planks";
        case 6:
            return "Sapling";
        case 7:
            return "Bedrock";
        case 8:
            return "Flowing Water";
        case 9:
            return "Water";
        case 10:
            return "Flowing Lava";
        case 11:
            return "Lava";
        case 12:
            return "Sand";
        case 13:
            return "Gravel";
        case 14:
            return "Gold Ore";
        case 15:
            return "Iron Ore";
        case 16:
            return "Coal Ore";
        case 17:
            return "Oak Wood";
        case 18:
            return "Leaves";
        case 19:
            return "Sponge";
        case 20:
            return "Glass";
        case 21:
            return "Lapis Lazuli Ore";
        case 22:
            return "Lapis Lazuli Block";
        case 23:
            return "Dispenser";
        case 24:
            return "Sandstone";
        case 25:
            return "Note Block";
        case 26:
            return "Bed";
        case 27:
            return "Powered Rail";
        case 28:
            return "Detector Rail";
        case 29:
            return "Sticky Piston";
        case 30:
            return "Cobweb";
        case 31:
            return "Tallgrass";
        case 32:
            return "Dead Bush";
        case 33:
            return "Piston";
        case 34:
            return "Piston Head";
        case 35:
            return "White Wool";
        //case 36:
        //    return "Cobweb";
        case 37:
            return "Dandelion";
        case 38:
            return "Rose";
        case 39:
            return "Brown Mushroom";
        case 40:
            return "Red Mushroom";
        case 41:
            return "Gold Block";
        case 42:
            return "Iron Block";
        case 43:
            return "Double Stone Slab";
        case 44:
            return "Stone Slab";
        case 45:
            return "Bricks";
        case 46:
            return "TNT";
        case 47:
            return "Bookshelf";
        case 48:
            return "Moss Stone";
        case 49:
            return "Obsidian";
        case 50:
            return "Torch";
        case 51:
            return "Fire";
        case 52:
            return "Monster Spawner";
        case 53:
            return "Oak Wood Stairs";
        case 54:
            return "Chest";
        case 55:
            return "Redstone Wire";
        case 56:
            return "Diamond Ore";
        case 57:
            return "Diamond Block";
        case 58:
            return "Crafting Table";
        case 59:
            return "Wheat Crops";
        case 60:
            return "Farmland";
        case 61:
            return "Furnace";
        case 62:
            return "Burning Furnace";
        case 63:
            return "Standing Sign";
        case 64:
            return "Oak Door";
        case 65:
            return "Ladder";
        case 66:
            return "Rail";
        case 67:
            return "Cobblestone Stairs";
        case 68:
            return "Wall-mounted Sign";
        case 69:
            return "Lever";
        case 70:
            return "Stone Pressure Plate";
        case 71:
            return "Iron Door";
        case 72:
            return "Wooden Pressure Plate";
        case 73:
            return "Redstone Ore";
        case 74:
            return "Glowing Redstone Ore";
        case 75:
            return "Redstone Torch (off)";
        case 76:
            return "Redstone Torch (on)";
        case 77:
            return "Stone Button";
        case 78:
            return "Snow Layer";
        case 79:
            return "Ice";
        case 80:
            return "Snow";
        case 81:
            return "Cactus";
        case 82:
            return "Clay";
        case 83:
            return "Reeds";
        case 84:
            return "Jukebox";
        case 85:
            return "Oak Fence";
        case 86:
            return "Pumpkin";
        case 87:
            return "Netherrack";
        case 88:
            return "Soul Sand";
        case 89:
            return "Glowstone";
        case 90:
            return "Nether Portal";
        case 91:
            return "Jack o' Lantern";
        case 92:
            return "Cake";
        case 93:
            return "Unpowered Redstone Repeater";
        case 94:
            return "Powered Redstone Repeater";
        case 95:
            return "Stained Glass";
        case 96:
            return "Trapdoor";
        default:
            return "Unknown";
    }
}