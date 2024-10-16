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
        bool lightSource;
        bool partialBlock;
        bool nonSolidBlock;
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

        bool getPartialBlock() {
            return partialBlock;
        };

        uint8_t getBlockLight() {
            return lightLevel;
        }

        uint8_t getSkyLight() {
            return skyLightLevel;
        }

        uint8_t getNonSolid() {
            return nonSolidBlock;
        }

        std::string getFacing() {
            /* True for:
             * - Torches (50)
             * - Furnace (61)
             * - Burning Furnace (62)
             * - Ladders (65)
             * - Redstone Torches (75/76)
             * - Pumpkins (86)
             * - Jack o'Lantern (91)
            */
            if (blockType == 50 || blockType == 61 ||blockType == 62 || blockType == 65 || blockType == 75 || blockType == 76 || blockType == 86 || blockType == 91) {
                switch(metaData) {
                    case 1:
                        return "South";
                    case 2:
                        return "North";
                    case 3:
                        return "West";
                    case 4:
                        return "East";
                    case 5:
                        return "Floor";
                }
            }
            return "No Direction";
        }

        std::string getName() {
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
                    return "Wood Plank";
                case 6:
                    return "Oak Sapling";
                case 7:
                    return "Bedrock";
                case 8:
                    return "Flowing Water";
                case 9:
                    return "Still Water";
                case 10:
                    return "Flowing Lava";
                case 11:
                    return "Still Lava";
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
                    return "Oak Leaves";
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
                    return "Dead Shrub/Grass";
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
                    return "Standing Sign Block";
                case 64:
                    return "Oak Door Block";
                case 65:
                    return "Ladder";
                case 66:
                    return "Rail";
                case 67:
                    return "Cobblestone Stairs";
                case 68:
                    return "Wall-mounted Sign Block";
                case 69:
                    return "Lever";
                case 70:
                    return "Stone Pressure Plate";
                case 71:
                    return "Iron Door Block";
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
                    return "Snow";
                case 79:
                    return "Snow";
                case 80:
                    return "Snow Block";
                case 81:
                    return "Cactus";
                case 82:
                    return "Clay";
                case 83:
                    return "Sugar Canes";
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
                    return "Jack o'Lantern";
                case 92:
                    return "Cake Block";
                case 93:
                    return "Redstone Repeater Block (off)";
                case 94:
                    return "Redstone Repeater Block (on)";
                default:
                    return "Unknown";
            }
        }
};