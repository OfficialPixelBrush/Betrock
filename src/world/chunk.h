#pragma once
#include "block.h"
#include "../compat.h"

class Chunk {
    public:
        Block blocks [16*128*16];
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
};