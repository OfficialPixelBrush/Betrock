#include "chunkBuilder.h"

bool ChunkBuilder::isSurrounded(uint x, uint y, uint z) {
    if (    
            chunk->getBlock(x-1,y,z)->getTransparent() ||
            chunk->getBlock(x+1,y,z)->getTransparent() ||
            chunk->getBlock(x,y-1,z)->getTransparent() ||
            chunk->getBlock(x,y+1,z)->getTransparent() ||
            chunk->getBlock(x,y,z-1)->getTransparent() ||
            chunk->getBlock(x,y,z+1)->getTransparent()
        )
    {
        return false;
    } else if (
            chunk->getBlock(x-1,y,z)->getBlockType() &&
            chunk->getBlock(x+1,y,z)->getBlockType() &&
            chunk->getBlock(x,y-1,z)->getBlockType() &&
            chunk->getBlock(x,y+1,z)->getBlockType() &&
            chunk->getBlock(x,y,z-1)->getBlockType() &&
            chunk->getBlock(x,y,z+1)->getBlockType()
        )
    {
        return true;
    }
    
    return false;
}

glm::vec2 ChunkBuilder::getBlockTextureOffset(unsigned char blockType) {
    float x = 0;
    float y = 0;
    const float divisor = 0.0625f;
    switch(blockType) {
        case 1: // Stone
            x=1;
            break;
        case 2: // Grass
            x=0;
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
        case 10: // Stationary Lava
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
        case 19: // Sponge
            x=0;
            y=3;
            break;
        case 20: // Glass
            x=1;
            y=3;
            break;
        case 21: // Lapis Lazuli Ore
            x=0;
            y=10;
        case 22: // Lapis Lazuli Block
            x=0;
            y=9;
            break;
        case 23: // Dispenser
            x=14;
            y=2;
            break;
        case 24: // Sandstone
            x=0;
            y=12;
            break;
        case 25: // Note Block
            x=10;
            y=4;
            break;
        case 26: // Bed
            x=7;
            y=8;
        case 27: // Powered Rail
            x=3;
            y=11;
            break;
        case 28: // Detector Rail
            x=3;
            y=12;
            break;
        case 29: // Sticky Piston
            x=10;
            y=6;
            break;
        case 30: // Cobweb
            x=11;
            y=0;
            break;
        case 31: // Grass
            x=7;
            y=2;
            break;
        case 83: // Sugar Canes
            x=9;
            y=4;
            break;
        default: // Missing Texture
            x=15;
            y=9;
            break;
    }
    return glm::vec2(x*divisor,-y*divisor);
}

Mesh* ChunkBuilder::getBlockModel(unsigned char blockType, uint x, uint y, uint z) {
    /*
    // Grass
    if (blockType == 2) {
        return &model->meshes[3];
    // Cross Model
    } else if (
        (blockType == 6) ||
        (blockType >= 30 && blockType <= 32) ||
        (blockType >= 37 && blockType <= 40) ||
        (blockType == 59) || (blockType == 83)) {
        return &model->meshes[1];
    // Fluids
    } else if (blockType >= 8 && blockType <= 11 && !chunk->getBlock(x,y+1,z)->getBlockType()) { 
        return &model->meshes[2];
    }
    */
    // Normal Block
    return &model->meshes[0];
}

glm::vec3 getBiomeBlockColor(unsigned char blockType, Vertex* vert) {
    glm::vec3 color = glm::vec3(0.57, 0.73, 0.34);
    if ((blockType == 2 && vert->normal.y > 0.0f) || (blockType == 18)) {
        return color;
    } else if (blockType == 31) {
        return color * 2.0f;
    }
    return glm::vec3(1.0f,1.0f,1.0f);
}

ChunkBuilder::ChunkBuilder(Model* model) {
    ChunkBuilder::model = model;
}

Mesh* ChunkBuilder::build(Chunk* chunk, int chunkX, int chunkZ) {
    ChunkBuilder::chunk = chunk;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    uint numberOfBlocks = 0;
    for (uint x = 0; x < 16; x++) {
        for (uint z = 0; z < 16; z++) {
            for (uint y = 0; y < 128; y++) {
                // Get next block to process
                Block* b = chunk->getBlock(x,y,z);
                unsigned char blockType = b->getBlockType();
                // Check if the block is air
                if (!b || b->getBlockType() == 0) {
                    continue;
                }
                // If the block is fully surrounded, don't bother loading it
                if (isSurrounded(x,y,z)) {
                    continue;
                }

                // Figure out the blocks coordinates in the world
                glm::vec3 pos = glm::vec3(float((chunkX*15)+x), float(y), float((chunkZ*15)+z));

                //std::cout << std::to_string(b->getBlockType()) << std::endl;
                Mesh* blockModel = getBlockModel(blockType, x,y,z);
                for (uint v = 0; v < blockModel->vertices.size(); v++) {
                    glm::vec3 color = getBiomeBlockColor(blockType, &blockModel->vertices[v]);
                    vertices.push_back(
                        Vertex(
                            glm::vec3(blockModel->vertices[v].position + pos),
                            blockModel->vertices[v].normal,
                            color,
                            blockModel->vertices[v].textureUV+getBlockTextureOffset(blockType)
                        )
                    );
                }

                GLuint totalVertices = vertices.size();
                for (uint i = 0; i < blockModel->indices.size(); i++) {
                    GLuint newInd = totalVertices + blockModel->indices[i];
                    indices.push_back(newInd);
                }
                numberOfBlocks++;
            }
        }
    }
    //std::cout << "Chunk at " << std::to_string(chunkX*15) << ", " << std::to_string(chunkZ*15) << " built" << std::endl;
    return new Mesh("chunk",vertices,indices,model->meshes[0].textures);
}