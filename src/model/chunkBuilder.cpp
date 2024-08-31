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

glm::vec2 ChunkBuilder::getBlockTextureOffset(unsigned char blockType, unsigned char blockMetaData) {
    float x = 0;
    float y = 0;
    const float divisor = 0.0625f;
                                                                                                                    // v Some sort of missing entry here!                   //v 50: torch
    uint8_t xBlock [] = { 0, 1, 0, 2, 0, 4,15, 1,15,15,15,15, 2, 3, 0, 1, 2, 4, 4, 0, 1, 0, 0,14, 0,10, 7, 3, 3,10,11, 7, 0, 7,12,11, 0,13,12,13,12, 7, 6, 5, 5, 7, 8, 3, 4, 5, 0,15, 1, 4,11, 5, 2, 8,11,15, 7,12,13, 4, 1, 3, 0, 0, 4, 0, 1, 2, 4, 3, 3, 3, 3, 1, 2, 3, 2, 6, 8, 9,11, 4, 7};
    uint8_t yBlock [] = { 0, 0, 0, 0, 1, 0, 0, 1,13,13,15,15, 1, 1, 2, 2, 2, 1, 3, 3, 3,10, 9, 2,12, 4, 8,11,12, 6, 0, 3, 0, 3, 6, 6, 4, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 5, 1, 4, 0, 1,10, 3, 1, 3, 5, 5, 2, 3, 0, 5, 5, 8, 1, 0, 6, 0, 5, 0, 3, 3, 7, 6, 0, 4, 4, 4, 4, 4, 4, 4, 0, 7};
    if (!blockMetaData) {
        return glm::vec2(float(xBlock[blockType])*divisor,-float(yBlock[blockType])*divisor);
    } else {
        switch(blockType) {
            // Logs
            case 17:
                if (blockMetaData == 1) {
                    x = 4;
                    y = 7;
                } else if (blockMetaData == 2) {
                    x = 5;
                    y = 7;
                }
                break;
            // Leaves
            case 18:
                if (blockMetaData == 1) {
                    x = 4;
                    y = 8;
                } else {
                    x = 4;
                    y = 3;
                }
                break;
            // Tallgrass
            case 31:
                if (blockMetaData == 1) {
                    x = 7;
                    y = 2;
                } else if (blockMetaData == 2) {
                    x = 8;
                    y = 3;
                }
                break;
            default:
                return glm::vec2(float(xBlock[blockType])*divisor,-float(yBlock[blockType])*divisor);
        }
        return glm::vec2(x*divisor,-y*divisor);
    }
}

uint8_t ChunkBuilder::getBlockModel(unsigned char blockType, uint x, uint y, uint z) {
    // Grass
    if (blockType == 2) {
        return 3;
    // Cross Model
    } else if (
        (blockType == 6) ||
        (blockType >= 30 && blockType <= 32) ||
        (blockType >= 37 && blockType <= 40) ||
        (blockType == 59) || (blockType == 83)) {
        return 1;
    // Fluids
    } else if (blockType >= 8 && blockType <= 11 && !chunk->getBlock(x,y+1,z)->getBlockType()) { 
        return 2;
    // Torches
    } else if (blockType == 50) {
        return 4;
    // Slab
    } else if (blockType == 44) {
        return 5;
    // Stair
    } else if (blockType == 53 || blockType == 67) {
        return 6;
    }
    // Normal Block
    return 0;
}

glm::vec3 getBiomeBlockColor(unsigned char blockType, Vertex* vert) {
    glm::vec3 color = glm::vec3(0.57, 0.73, 0.34);
    if ((blockType == 2 && vert->normal.y > 0.0f) || (blockType == 18) || (blockType == 31)) {
        return color;
    }
    return glm::vec3(1.0f,1.0f,1.0f);
}

ChunkBuilder::ChunkBuilder(Model* model) {
    ChunkBuilder::model = model;
}

float getLighting(Chunk* chunk, int x, int y, int z, glm::vec3 normal) {
    float lightArray[16] = {0.035f,0.044f,0.055f,0.069f,0.086f,0.107f,0.134f,0.168f,0.21f,0.262f,0.328f,0.41f,0.512f,0.64f,0.8f,1.0f};
    Block* b = chunk->getBlock( x + int(normal.x),
                                y + int(normal.y),
                                z + int(normal.z));
    int light = b->getBlockLight() + b->getSkyLight();
    return lightArray[std::min(15,light)];
}

uint8_t isVisible(Chunk* chunk, int x, int y, int z, uint8_t blockModelIndex, glm::vec3 normal) {
    Block* b = chunk->getBlock( x + int(normal.x),
                                y + int(normal.y),
                                z + int(normal.z));
    // Change || to an && for Optifine Smart trees
    if (b->getTransparent() || blockModelIndex!=0) {
        return 0;
    } else {
        return b->getBlockType();
    }
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
                unsigned char blockMetaData = b->getBlockMetaData();
                // Check if the block is air
                if (!b || b->getBlockType() == 0) {
                    continue;
                }
                // If the block is fully surrounded, don't bother loading it
                if (isSurrounded(x,y,z)) {
                    continue;
                }

                // Figure out the blocks coordinates in the world
                glm::vec3 pos = glm::vec3(float((chunkX*16)+x), float(y), float((chunkZ*16)+z));

                //std::cout << std::to_string(b->getBlockType()) << std::endl;
                uint8_t blockModelIndex = getBlockModel(blockType, x,y,z);
                Mesh* blockModel = &model->meshes[blockModelIndex];
                for (uint v = 0; v < blockModel->vertices.size(); v++) {
                    if (isVisible(chunk,x,y,z,blockModelIndex,blockModel->vertices[v].normal)) {
                        continue;
                    }
                    glm::vec3 color = getBiomeBlockColor(blockType, &blockModel->vertices[v]);
                    // TODO: Fix BlockLight
                    color *= getLighting(chunk,x,y,z,blockModel->vertices[v].normal);
                    //std::cout << std::to_string(b->getBlockLight()) << std::endl;

                    vertices.push_back(
                        Vertex(
                            glm::vec3(blockModel->vertices[v].position + pos),
                            blockModel->vertices[v].normal,
                            color,
                            blockModel->vertices[v].textureUV+getBlockTextureOffset(blockType,blockMetaData)
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