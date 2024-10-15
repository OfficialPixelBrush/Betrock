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

        std::string getFacing() {
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
                default:
                    return "";
            }
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
                case 50:
                    return "Torch";
                case 65:
                    return "Ladder";
                default:
                    return "Unknown";
            }
        }
};