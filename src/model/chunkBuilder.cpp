#include "chunkBuilder.h"
#include <stdexcept>  // For std::runtime_error

#include <stdexcept>  // For std::runtime_error

const float lightArray[16] = {0.035f, 0.044f, 0.055f, 0.069f, 0.086f, 0.107f, 0.134f, 0.168f, 0.21f, 0.262f, 0.328f, 0.41f, 0.512f, 0.64f, 0.8f, 1.0f};

bool ChunkBuilder::isSurrounded(int x, int y, int z, uint8_t blockIndex) {
    bool onlySurroundedBySame = false;
    try {
        if (blockIndex >= 8 && blockIndex <= 11) {
            onlySurroundedBySame = true;
        }
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

        if (onlySurroundedBySame) {
            // Check if all adjacent blocks have a block type
            for (int i = 0; i < 6; ++i) {
                if (blocks[i]->getBlockType() == blockIndex) {
                    return false;
                }
            }
        } else {
        // Check if all adjacent blocks have a block type
            for (int i = 0; i < 6; ++i) {
                if (blocks[i]->getBlockType() == 0 || blocks[i]->getPartialBlock()) {
                    return false;
                }
            }
        }

        return true;

    } catch (const std::exception& e) {
        return false;
    }
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
        int light = 0;
        int relevantLights = 0;
        Block* b;

        // Get the adjacent blocks along face
        for (float aOff = -0.5; aOff <= 0.5; aOff++) {
            for (float bOff = -0.5; bOff <= 0.5; bOff++) {
                if (normal.x) {
                    b = world->getBlock(floor(x+normal.x*0.5f), floor(y+aOff), floor(z+bOff));
                }
                if (normal.y) {
                    b = world->getBlock(floor(x+aOff), floor(y+normal.y*0.5f), floor(z+bOff));
                }
                if (normal.z) {
                    b = world->getBlock(floor(x+aOff), floor(y+bOff), floor(z+normal.z*0.5f));
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


uint8_t isHidden(World* world, int x, int y, int z, Block* currentBlock, glm::vec3 normal) {
    if (currentBlock == nullptr) {
        return true;
    }
    // Calculate adjacent block coordinates
    int adjX = x + static_cast<int>(normal.x);
    int adjY = y + static_cast<int>(normal.y);
    int adjZ = z + static_cast<int>(normal.z);

    // Get the adjacent block
    Block* adjacentBlock = world->getBlock(adjX, adjY, adjZ);
    // No adjacent Block, not hidden
    if (adjacentBlock == nullptr) {
        return false;
    }

    // If it's the same as the checking block
    if (currentBlock->getBlockType() >= 8 && currentBlock->getBlockType() <= 11 || (!currentBlock->getPartialBlock() && currentBlock->getTransparent() && currentBlock->getBlockType() != 18)) {
        if (adjacentBlock->getBlockType() == currentBlock->getBlockType() && currentBlock->getBlockMetaData() == 0) {
            return true;
        }
    } else {
        if (adjacentBlock->getTransparent() || adjacentBlock->getPartialBlock() || currentBlock->getPartialBlock()) {
            return false;
        } else {
            return adjacentBlock->getBlockType();
        }
    }
    return false;
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    
    return result;
}

Mesh* ChunkBuilder::getBlockMesh(uint8_t blockType, int x, int y, int z, uint8_t blockMetaData) {

    std::string specialQuery = "";
    if (blockType == 0) {
        return nullptr;
    }

    // Bed
    if (blockType == 26) {
        // Check top-most bit to check if head of bed
        if (blockMetaData & 0x08) {
            specialQuery = "Head";
        } else {
            specialQuery = "End";
        }
    }

    std::vector<std::string> compareTo;
    // Check Cached Mesh first to save time
    if (cachedMesh != nullptr) {
        compareTo = splitString(cachedMesh->name,'_');
        if (blockType == std::stoi(compareTo[0]) && blockMetaData == std::stoi(compareTo[1])) {
            return cachedMesh;
        }
    }
    
    // Search for the mesh
    for (auto& m : model->meshes) {
        compareTo = splitString(m.name,'_');
        if (blockType == std::stoi(compareTo[0])) {
            // TODO: Temp while some metadata situations aren't accounted for
            if (blockType == 18 || blockType == 17 || blockType == 31 || blockType == 50) {
                if (blockMetaData == std::stoi(compareTo[1])) {
                    return &m;
                } else {
                    continue;
                }
            }
            
            if (specialQuery == "") {
                return &m;
            }
            if (specialQuery == compareTo[3]) {
                return &m;
            }
        }
    }
    return &model->meshes[0];
}

std::vector<ChunkMesh*> ChunkBuilder::buildChunks(std::vector<Chunk*> chunks, bool smoothLighting, uint8_t maxSkyLight) {
    std::vector<ChunkMesh*> meshes;
    for (auto c : chunks) {
        meshes.push_back(buildChunk(c,smoothLighting,maxSkyLight));
    }
    return meshes;
}

ChunkMesh* ChunkBuilder::buildChunk(Chunk* chunk, bool smoothLighting, uint8_t maxSkyLight) {
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
                // Check if the block is air
                if (!b || b == nullptr) {
                    continue;
                }
                unsigned char blockType = b->getBlockType();
                if (blockType == 0) {
                    continue;
                }
                // If the block is fully surrounded, don't bother loading it
                if (isSurrounded(x,y,z,blockType)) {
                    continue;
                }
                unsigned char blockMetaData = b->getBlockMetaData();

                // Figure out the blocks coordinates in the world
                glm::vec3 pos = glm::vec3(float(x), float(y), float(z));

                cachedMesh = getBlockMesh(blockType,x,y,z,blockMetaData);
                Mesh* blockModel = cachedMesh;
                if (!blockModel) {
                    continue;
                }
                glm::vec3 offset;
                for (uint v = 0; v < blockModel->vertices.size(); v++) {
                    if (isHidden(world,x,y,z,b,blockModel->vertices[v].normal)) {
                        continue;
                    }
                    glm::vec3 color = getBiomeBlockColor(blockType, blockMetaData, &blockModel->vertices[v]);

                    // Liquid change model
                    // If we're dealing with the upper verts of  water or lava
                    if (blockType >= 8 && blockType <= 11 && blockModel->vertices[v].position.y > 0.2f) {
                        Block* wb = world->getBlock(x,y+1,z);
                        // Offset liquid height based on metadata
                        if (wb->getBlockType() != blockType) {
                            switch(blockMetaData) {
                                case 1:
                                    offset = glm::vec3(0.0,-1.0+0.75 ,0.0);
                                    break;
                                case 2:
                                    offset = glm::vec3(0.0,-1.0+0.75 ,0.0);
                                    break;
                                case 3:
                                    offset = glm::vec3(0.0,-1.0+0.625,0.0);
                                    break;
                                case 4:
                                    offset = glm::vec3(0.0,-1.0+0.5  ,0.0);
                                    break;
                                case 5:
                                    offset = glm::vec3(0.0,-1.0+0.375,0.0);
                                    break;
                                case 6:
                                    offset = glm::vec3(0.0,-1.0+0.25 ,0.0);
                                    break;
                                case 7:
                                    offset = glm::vec3(0.0,-1.0+0.125,0.0);
                                    break;
                                default:
                                    offset = glm::vec3(0.0,-(1.0/8.0),0.0);
                                    break;
                            }
                        }
                    } else {
                        offset = glm::vec3(0.0,0.0,0.0);
                    }

                    glm::vec3 finalPos = glm::vec3(blockModel->vertices[v].position + pos + offset);
                    glm::vec2 finalUV = blockModel->vertices[v].textureUV;
                    // Only affects the side
                    if (blockModel->vertices[v].normal.y == 0) {
                        finalUV.y = finalUV.y + (offset.y/16);
                    }
                    
                    // Water in it's own thing
                    if (blockType == 8 || blockType == 9) {
                        color *= getLighting(world,x,y,z,blockModel->vertices[v].normal, maxSkyLight);
                        waterVertices.push_back(
                            Vertex(
                                finalPos,
                                blockModel->vertices[v].normal,
                                color,
                                finalUV
                            )
                        );
                    } else {
                        // Apply light if the block isn't a lightsource
                        if (!b->lightSource) {
                            if (smoothLighting) {
                                color *= getSmoothLighting(world,finalPos,blockModel->vertices[v].normal,maxSkyLight);
                            } else {
                                color *= getLighting(world,x,y,z,blockModel->vertices[v].normal, maxSkyLight);
                            }
                        }
                        worldVertices.push_back(
                            Vertex(
                                finalPos,
                                blockModel->vertices[v].normal,
                                color,
                                finalUV
                            )
                        );
                    }
                }

                GLuint totalWaterIndices = waterIndices.size();
                GLuint totalWorldIndices = worldIndices.size();

                if (blockType == 8 || blockType == 9) {
                    for (uint i = 0; i < blockModel->indices.size(); i++) {
                        GLuint newInd = totalWaterIndices + blockModel->indices[i];
                        waterIndices.push_back(newInd);
                    }
                } else {
                    for (uint i = 0; i < blockModel->indices.size(); i++) {
                        GLuint newInd = totalWorldIndices + blockModel->indices[i];
                        worldIndices.push_back(newInd);
                    }
                }
            }
        }
    }
    std::vector<Mesh*> meshes;
    std::cout << "Making meshes" << std::endl;
    // TODO: Figure out why this segfaults under thread
    meshes.push_back(new Mesh("world", worldVertices, worldIndices, model->meshes[0].textures));
    meshes.push_back(new Mesh("water", waterVertices, waterIndices, model->meshes[0].textures));
    std::cout << "Returning" << std::endl;
    return new ChunkMesh(chunk,meshes);
}