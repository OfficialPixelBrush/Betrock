#pragma once
//#include "chunk.h"
#include "regionLoader.h"

class region {
    public:
        int x,z = 0;
        region(int pX, int pZ) {
            x = pX;
            z = pZ;
            //chunks = rd.decode(x,z);
            regionLoader rL;
            if (rL.loadRegion(x,z)) { //(chunks == NULL) {
                cout << "Issue loading Region!" << endl;
            }
            cout << "Loaded successfully!" << endl;
            //delete rL;
        }
    private:
        //chunk chunks [32*32];
};