#pragma once
#include "../debug.h"
#include "block.h"
#include "../compat.h"
class Chunk {
    Block blocks [16*128*16];
    public:
        int x,z;
        Chunk(int pX, int pZ, int8_t pBlockData [], int8_t pSkyLightData[], int8_t pBlockLightData [], int8_t pMetaData []) {
            x = pX;
            z = pZ;
            for (uint i = 0; i < 16*128*16; i++) {
                uint8_t blockLight = pBlockLightData[i/2];
                uint8_t skyLight = pSkyLightData[i/2];
                uint8_t metaData = pMetaData[i/2];
                if (i%2) {
                    blockLight = blockLight >> 4;
                    skyLight = skyLight >> 4;
                    metaData = metaData >> 4;
                } else {
                    blockLight = blockLight & 0xF;
                    skyLight = skyLight & 0xF;
                    metaData = metaData & 0xF;
                }
                blocks[i] = Block(pBlockData[i], skyLight, blockLight, metaData);
            }
            //std::cout << "Created Chunk at " << x << ", " << z << std::endl;
        }

        ~Chunk() {
            std::cout << "Unloaded Chunk at " << x << ", " << z << std::endl;
        }

        Block* getAllBlocks() {
            return blocks;
        }

        Block* getBlock(int x, uint y, int z) {
            if (y > 127) {
                return nullptr;
            }

            if (logoDebug) {
                int logo[5][28] = {
                    {1,1,0,0,1,1,1,0,0,1,0,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,0,1},
                    {1,0,1,0,1,0,0,0,0,1,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0},
                    {1,1,0,0,1,1,0,0,0,1,0,0,1,1,0,0,1,0,1,0,1,0,0,0,1,1,0,0},
                    {1,0,1,0,1,0,0,0,0,1,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0},
                    {1,1,0,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,0,1}
                };
                
                if (x >= 5 || z >= 28 || x < 0 || z < 0) {
                    return new Block(AIR);
                }

                if (y == 1 && logo[x][z] == 1) {
                    return new Block(COBBLESTONE);
                } else {
                    return new Block(AIR);
                }
            }

            x = x%16;
            z = z%16;
            if (x < 0) {
                x+=16;
            }
            if (z < 0) {
                z+=16;
            }
            return &blocks[y + z*128 + (x*128*16)];
        }
};