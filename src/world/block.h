#pragma once
#include "../global.h"
#include "../compat.h"

enum BLOCK {
    AIR,
    STONE,
    GRASS,
    DIRT,
    COBBLESTONE,
    PLANKS,
    SAPLING,
    BEDROCK,
    FLOWING_WATER,
    WATER,
    FLOWING_LAVA,
    LAVA,
    SAND,
    GRAVEL,
    GOLD_ORE,
    IRON_ORE,
    COAL_ORE,
    LOG,
    LEAVES,
    SPONGE,
    GLASS,
    LAPIS_ORE,
    LAPIS_BLOCK,
    DISPENSER,
    SANDSTONE,
    NOTEBLOCK,
    BED,
    GOLDEN_RAIL,
    DETECTOR_RAIL,
    STICKY_PISTON,
    WEB,
    TALLGRASS,
    DEADBUSH,
    PISTON,
    PISTON_HEAD,
    WOOL,
    MISSING_BLOCK_36,
    YELLOW_FLOWER,
    RED_FLOWER,
    BROWN_MUSHROOM,
    RED_MUSHROOM,
    GOLD_BLOCK,
    IRON_BLOCK,
    DOUBLE_STONE_SLAB,
    STONE_SLAB,
    BRICKS,
    TNT,
    BOOKSHELF,
    MOSSY_COBBLESTONE,
    OBSIDIAN,
    TORCH,
    FIRE,
    MOB_SPAWNER,
    OAK_STAIRS,
    CHEST,
    REDSTONE_WIRE,
    DIAMOND_ORE,
    DIAMOND_BLOCK,
    CRAFTING_TABLE,
    WHEAT,
    FARMLAND,
    FURNACE,
    LIT_FURNACE,
    STANDING_SIGN,
    WOODEN_DOOR,
    LADDER,
    RAIL,
    STONE_STAIRS,
    WALL_SIGN,
    LEVER,
    STONE_PRESSURE_PLATE,
    IRON_DOOR,
    WOODEN_PRESSURE_PLATE,
    REDSTONE_ORE,
    LIT_REDSTONE_ORE,
    UNLIT_REDSTONE_TORCH,
    REDSTONE_TORCH,
    STONE_BUTTON,
    SNOW_LAYER,
    ICE,
    SNOW,
    CACTUS,
    CLAY,
    REEDS,
    JUKEBOX,
    FENCE,
    PUMPKIN,
    NETHERRACK,
    SOUL_SAND,
    GLOWSTONE,
    PORTAL,
    LIT_PUMPKIN,
    CAKE,
    UNPOWERED_REPEATER,
    POWERED_REPEATER,
    STAINED_GLASS,
    TRAPDOOR
};

class Block {
    public:
        uint8_t blockType;
        uint8_t metaData;
        uint8_t skyLightLevel;
        uint8_t lightLevel;
        Block(uint8_t blockType = AIR, uint8_t skyLightLevel = 15, uint8_t lightLevel = 15, uint8_t metaData = 0);

        uint8_t getBlockType() {
            return blockType;
        };

        uint8_t getMetaData() {
            return metaData;
        }

        uint8_t getBlockLight() {
            return lightLevel;
        }

        uint8_t getSkyLight() {
            return skyLightLevel;
        }
};