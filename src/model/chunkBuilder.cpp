#include "chunkBuilder.h"

bool isSurrounded(Chunk* chunk, uint x, uint y, uint z) {
    if (chunk->getBlock(x-1,y,z)->getBlockType() &&
        chunk->getBlock(x+1,y,z)->getBlockType() &&
        chunk->getBlock(x,y-1,z)->getBlockType() &&
        chunk->getBlock(x,y+1,z)->getBlockType() &&
        chunk->getBlock(x,y,z-1)->getBlockType() &&
        chunk->getBlock(x,y,z+1)->getBlockType() &&

        !chunk->getBlock(x-1,y,z)->getTransparent() &&
        !chunk->getBlock(x+1,y,z)->getTransparent() &&
        !chunk->getBlock(x,y-1,z)->getTransparent() &&
        !chunk->getBlock(x,y+1,z)->getTransparent() &&
        !chunk->getBlock(x,y,z-1)->getTransparent() &&
        !chunk->getBlock(x,y,z+1)->getTransparent())
    {
        
        return true;
    }
    return false;
}

glm::vec2 getBlockTextureOffset(unsigned char blockType) {
    float x = 0;
    float y = 0;
    const float divisor = 0.0625f;
    switch(blockType) {
        case 1: // Stone
            x=1;
            break;
        case 2: // Grass
            x=3;
            break;
        case 3: // Dirt
            x=2;
            break;
        case 4: // Cobblestone
            y=1;
            break;
        case 5: // Oak Wood Plank
            x=4;
            break;
        case 6: // Oak Sapling
            x=15;
            break;
        case 7: // Bedrock
            x=1;
            y=1;
            break;
        case 8: // Flowing Water
            x=15;
            y=13;
            break;
        case 9: // Still Water
            x=15;
            y=13;
            break;
        case 10: // Stationary LAva
            x=15;
            y=15;
            break;
        case 11: // Still LAva
            x=15;
            y=15;
            break;
        case 12: // Sand
            x=2;
            y=1;
            break;
        case 13: // Gravel
            x=3;
            y=1;
            break;
        case 14: // Gold Ore
            x=3;
            y=1;
            break;
        case 15: // Iron Ore
            x=1;
            y=2;
            break;
        case 16: // Coal Ore
            x=2;
            y=2;
            break;
        case 17: // Oak Wood
            x=4;
            y=1;
            break;
        case 18: // Oak Leaves
            x=4;
            y=3;
            break;
        default: // Missing Texture
            x=15;
            y=9;
            break;
    }
    return glm::vec2(x*divisor,-y*divisor);
}

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
                if (!b || b->getBlockType() == 0) {
                    continue;
                }

                if (isSurrounded(chunk, x,y,z)) {
                    continue;
                }
                //std::cout << std::to_string(b->getBlockType()) << std::endl;
                for (uint v = 0; v < blockModel->vertices.size(); v++) {
                    Vertex* original = &blockModel->vertices[v];
                    Vertex newVert(glm::vec3(original->position + pos), original->color, original->normal, original->textureUV+getBlockTextureOffset(b->getBlockType()));
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
    return new Mesh("chunk",vertices,indices,tex);
}