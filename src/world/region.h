#pragma once
//#include "chunk.h"
#include "regionLoader.h"
#include "../compat.h"

class region {
    public:
        int x,z = 0;
        chunk* chunks;
        region(int pX, int pZ) {
            x = pX;
            z = pZ;
            //chunks = rd.decode(x,z);
            regionLoader rL;
            chunks = rL.loadRegion(x,z);
            //block* blocks = chunks[16].getData();
            //std::cout << std::to_string(blocks[0].getBlock()) << std::endl;
            if (!chunks) {
                cout << "Issue loading Region!" << endl;
            } else {
                cout << "Loaded successfully!" << endl;
            }
            //delete rL;
        }

        // TODO: Implement coordinate-based chunk-getting
        chunk* getChunk() {
            return &chunks[0];
        }
    private:
        //chunk chunks [32*32];
};