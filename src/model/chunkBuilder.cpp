#include "chunkBuilder.h"

Mesh* ChunkBuilder::build(Mesh* blockModel, Chunk* chunk, std::vector <Texture> tex) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    uint numberOfBlocks = 0;
    for (uint x = 0; x < 16; x++) {
        for (uint z = 0; z < 16; z++) {
            for (uint y = 0; y < 128; y++) {
                Block* b = chunk->getBlock(x,y,z);
                //std::cout << std::to_string(x) << ", " << std::to_string(y) << ", " << std::to_string(z) << ": ";
                glm::vec3 pos = glm::vec3(float(x), float(y), float(z));
                // If there's no block, move on
                if (!b) {
                    continue;
                }
                if (b->getBlock() == 0) {
                    continue;
                }
                //std::cout << std::to_string(b->getBlock()) << std::endl;
                for (uint v = 0; v < blockModel->vertices.size(); v++) {
                    Vertex* original = &blockModel->vertices[v];
                    Vertex newVert(glm::vec3(original->position + pos), original->color, original->normal);
                    vertices.push_back(newVert);
                }
                for (uint v = 0; v < blockModel->indices.size(); v++) {
                    GLuint newInd = (blockModel->vertices.size()*numberOfBlocks) + blockModel->indices[v];
                    indices.push_back(newInd);
                }
                numberOfBlocks++;
            }
        }
    }
    return new Mesh(vertices,indices,tex);
}