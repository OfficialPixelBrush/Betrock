#include "../render/mesh.h"
#include <vector>

class chunkMesh {
    // Mesh 0 is terrain
    // Mesh 1 is water/semi-transparent
    public:
        std::vector<Mesh> meshes;
};