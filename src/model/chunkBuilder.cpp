#include "chunkBuilder.h"
#include <stdexcept>  // For std::runtime_error

#include <stdexcept>  // For std::runtime_error

const float lightArray[16] = {0.035f, 0.044f, 0.055f, 0.069f, 0.086f, 0.107f, 0.134f, 0.168f, 0.21f, 0.262f, 0.328f, 0.41f, 0.512f, 0.64f, 0.8f, 1.0f};

bool ChunkBuilder::isSurrounded(int x, int y, int z) {
    try {
        // Cache block pointers
        Block* blocks[6] = {
            world->getBlock(x - 1, y, z),
            world->getBlock(x + 1, y, z),
            world->getBlock(x, y - 1, z),
            world->getBlock(x, y + 1, z),
            world->getBlock(x, y, z - 1),
            world->getBlock(x, y, z + 1)
        };

        // Check if any adjacent block is transparent
        for (int i = 0; i < 6; ++i) {
            if (blocks[i] == nullptr) {
                throw std::runtime_error("Block pointer is null.");
            }
            if (blocks[i]->getTransparent()) {
                return false;
            }
        }

        // Check if all adjacent blocks have a block type
        for (int i = 0; i < 6; ++i) {
            if (blocks[i]->getBlockType() == 0) {
                return false;
            }
        }

        return true;

    } catch (const std::exception& e) {
        return false;
    }
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

uint8_t ChunkBuilder::getBlockModel(unsigned char blockType, int x, int y, int z) {
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
    } else if (blockType >= 8 && blockType <= 11 && !world->getBlock(x,y+1,z)->getBlockType()) { 
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
    // Fence
    } else if (blockType == 85) {
        return 7;
    // Snow Layer
    } else if (blockType == 78) {
        return 8;
    }
    // Normal Block
    return 0;
}

glm::vec3 getBiomeBlockColor(unsigned char blockType, unsigned char blockMetaData, Vertex* vert) {
    glm::vec3 color = glm::vec3(0.57, 0.73, 0.34);
    // Biome Colored
    if ((blockType == 2 && vert->normal.y > 0.0f) || (blockType == 31)) {
        return color;
    } else if (blockType == 18) {
        // Spruce
        if (blockMetaData == 1) {
            return glm::vec3(0.38,0.6,0.38);    
        // Birch
        } else if (blockMetaData == 2) {
            return glm::vec3(0.502,0.655,0.333);
        } else {
            return color;
        }
    }
    // Ignore Biome Coloring
    return glm::vec3(1.0f,1.0f,1.0f);
}

ChunkBuilder::ChunkBuilder(Model* model, World* world) {
    ChunkBuilder::model = model;
    ChunkBuilder::world = world;
}

float getLighting(World* world, int x, int y, int z, glm::vec3 normal, uint8_t maxSkyLight) {
    // Array for light values

    try {
        // Calculate adjacent block coordinates
        int adjX = x + static_cast<int>(normal.x);
        int adjY = y + static_cast<int>(normal.y);
        int adjZ = z + static_cast<int>(normal.z);

        // Get the adjacent block
        Block* adjacentBlock = world->getBlock(adjX, adjY, adjZ);
        if (adjacentBlock == nullptr) {
            throw std::runtime_error("Adjacent block pointer is null.");
        }

        // Get lighting values from the adjacent block
        int light = adjacentBlock->getBlockLight() + std::min(adjacentBlock->getSkyLight(), maxSkyLight);

        // Ensure light index is within bounds
        if (light < 0) {
            light = 0;
        } else if (light > 15) {
            light = 15;
        }

        return lightArray[light];

    } catch (const std::exception& e) {
        return 1.0f;  // Return a default value in case of an error
    }
}

float getSmoothLighting(World* world, glm::vec3 position, glm::vec3 normal, uint8_t maxSkyLight) {
    // Array for light values

    try {
        float x = position.x;
        float y = position.y;
        float z = position.z;
        // Calculate adjacent block coordinates
        /*
        int adjX = x + static_cast<int>(normal.x);
        int adjY = y + static_cast<int>(normal.y);
        int adjZ = z + static_cast<int>(normal.z);
        */
        int light = 0;
        int relevantLights = 0;
        Block* b;

        // Get the adjacent block
        for (float aOff = -0.5; aOff <= 0.5; aOff+=1.0f) {
            for (float bOff = -0.5; bOff <= 0.5; bOff+=1.0f) {
                if (normal.x) {
                    b = world->getBlock(int(x+normal.x*0.5f), int(y+aOff), int(z+bOff));
                }
                if (normal.y) {
                    b = world->getBlock(int(x+aOff), int(y+normal.y*0.5f), int(z+bOff));
                    //std::cout << "NormY: " << normal.y << std::endl;
                }
                if (normal.z) {
                    b = world->getBlock(int(x+aOff), int(y+bOff), int(z+normal.z*0.5f));
                }
                if (b) {
                    // Air is transparent, so we can ignore it too
                    if (b->getBlockType() == 0 || b->getTransparent()) {
                        light += std::max(b->getBlockLight(), std::min(b->getSkyLight(), maxSkyLight));
                        relevantLights++;
                    }
                }
            }
        }
        /*
        b = world->getBlock((int)(x+0.5), (int)(y+0.5), (int)(z-0.5));
        if (b) {
            if (b->getBlockType() == 0 || b->getTransparent()) {
                light += std::max(b->getBlockLight(), std::min(b->getSkyLight(), maxSkyLight));
                relevantLights++;
            }
        }
        b = world->getBlock((int)(x+0.5), (int)(y+0.5), (int)(z-0.5));
        if (b) {
            if (b->getBlockType() == 0 || b->getTransparent()) {
                light += std::max(b->getBlockLight(), std::min(b->getSkyLight(), maxSkyLight));
                relevantLights++;
            }
        }
        b = world->getBlock((int)(x+0.5), (int)(y+0.5), (int)(z-0.5));
        if (b) {
            if (b->getBlockType() == 0 || b->getTransparent()) {
                light += std::max(b->getBlockLight(), std::min(b->getSkyLight(), maxSkyLight));
                relevantLights++;
            }
        }*/

        if (!relevantLights) {
            return lightArray[0];
        }
        light = light / relevantLights;
        /*
        if (adjacentBlock == nullptr) {
            throw std::runtime_error("Adjacent block pointer is null.");
        }

        // Get lighting values from the adjacent block
        int light = adjacentBlock->getBlockLight() + std::min(adjacentBlock->getSkyLight(), maxSkyLight);
        */
        // Ensure light index is within bounds
        if (light < 0) {
            light = 0;
        } else if (light > 15) {
            light = 15;
        }

        return lightArray[light];

    } catch (const std::exception& e) {
        return 1.0f;  // Return a default value in case of an error
    }
}


uint8_t isVisible(World* world, int x, int y, int z, uint8_t blockModelIndex, glm::vec3 normal) {
    try {
        // Calculate adjacent block coordinates
        int adjX = x + static_cast<int>(normal.x);
        int adjY = y + static_cast<int>(normal.y);
        int adjZ = z + static_cast<int>(normal.z);

        // Get the adjacent block
        Block* adjacentBlock = world->getBlock(adjX, adjY, adjZ);
        if (adjacentBlock == nullptr) {
            throw std::runtime_error("Adjacent block pointer is null.");
        }

        // Check if the adjacent block is transparent
        if (adjacentBlock->getTransparent()) {
            // Get the current block
            Block* currentBlock = world->getBlock(x, y, z);
            if (currentBlock == nullptr) {
                throw std::runtime_error("Current block pointer is null.");
            }

            // Check if the current block is of a certain type and visible
            if ((currentBlock->getBlockType() >= 8 && currentBlock->getBlockType() <= 11) ||
                currentBlock->getBlockType() == 20) {
                if (adjacentBlock->getBlockType() == currentBlock->getBlockType()) {
                    return adjacentBlock->getBlockType();
                }
            }
            return 0;  // Not visible
        } else {
            return adjacentBlock->getBlockType();
        }
    } catch (const std::exception& e) {
        return 0;  // Not visible or error case
    }
}

std::vector<ChunkMesh*> ChunkBuilder::buildChunks(std::vector<Chunk*> chunks, uint8_t maxSkyLight) {
    std::vector<ChunkMesh*> meshes;
    for (auto c : chunks) {
        meshes.push_back(buildChunk(c,maxSkyLight));
    }
    return meshes;
}

ChunkMesh* ChunkBuilder::buildChunk(Chunk* chunk, uint8_t maxSkyLight) {
    std::vector<Vertex> worldVertices;
    std::vector<GLuint> worldIndices;

    std::vector<Vertex> waterVertices;
    std::vector<GLuint> waterIndices;

    if (chunk == nullptr) {
        return nullptr;
    }
    int chunkX = chunk->x*16;
    int chunkZ = chunk->z*16;

    //std::cout << "Chunk" << " " << chunk->x << ", " << chunk->z << std::endl;
    for (int x = chunkX; x < 16+chunkX; x++) {
        for (int z = chunkZ; z < 16+chunkZ; z++) {
            for (uint y = 0; y < 128; y++) {
                // Get next block to process
                Block* b = world->getBlock(x,y,z);
                unsigned char blockType = b->getBlockType();
                unsigned char blockMetaData = b->getBlockMetaData();
                // Check if the block is air
                if (!b || blockType == 0) {
                    continue;
                }
                // If the block is fully surrounded, don't bother loading it
                if (isSurrounded(x,y,z)) {
                    continue;
                }

                // Figure out the blocks coordinates in the world
                glm::vec3 pos = glm::vec3(float(x), float(y), float(z));

                //std::cout << std::to_string(b->getBlockType()) << std::endl;
                uint8_t blockModelIndex = getBlockModel(blockType, x,y,z);
                Mesh* blockModel = &model->meshes[blockModelIndex];
                for (uint v = 0; v < blockModel->vertices.size(); v++) {
                    if (isVisible(world,x,y,z,blockModelIndex,blockModel->vertices[v].normal) && !b->getPartialBlock()) {
                        continue;
                    }
                    glm::vec3 color = getBiomeBlockColor(blockType, blockMetaData, &blockModel->vertices[v]);
                    glm::vec3 finalPos = glm::vec3(blockModel->vertices[v].position + pos);
                    // TODO: Fix BlockLight
                    // color *= getLighting(world,x,y,z,blockModel->vertices[v].normal, maxSkyLight);
                    //std::cout << std::to_string(b->getBlockLight()) << std::endl;
                    if (blockType == 8 || blockType == 9) {
                        color *= getLighting(world,x,y,z,blockModel->vertices[v].normal, maxSkyLight);
                        waterVertices.push_back(
                            Vertex(
                                finalPos,
                                blockModel->vertices[v].normal,
                                color,
                                blockModel->vertices[v].textureUV+getBlockTextureOffset(blockType,blockMetaData)
                            )
                        );
                    } else {
                        if (!b->lightSource) {
                            color *= getSmoothLighting(world,finalPos,blockModel->vertices[v].normal,maxSkyLight);
                        }
                        worldVertices.push_back(
                            Vertex(
                                finalPos,
                                blockModel->vertices[v].normal,
                                color,
                                blockModel->vertices[v].textureUV+getBlockTextureOffset(blockType,blockMetaData)
                            )
                        );
                    }
                }

                if (blockType == 8 || blockType == 9) {
                    GLuint totalVertices = waterVertices.size();
                    for (uint i = 0; i < blockModel->indices.size(); i++) {
                        GLuint newInd = totalVertices + blockModel->indices[i];
                        waterIndices.push_back(newInd);
                    }
                } else {
                    GLuint totalVertices = worldVertices.size();
                    for (uint i = 0; i < blockModel->indices.size(); i++) {
                        GLuint newInd = totalVertices + blockModel->indices[i];
                        worldIndices.push_back(newInd);
                    }
                }
            }
        }
    }
    std::vector<Mesh*> meshes;
    meshes.push_back(new Mesh("world", worldVertices, worldIndices, model->meshes[0].textures));
    meshes.push_back(new Mesh("water", waterVertices, waterIndices, model->meshes[0].textures));
    return new ChunkMesh(chunk,meshes);
}