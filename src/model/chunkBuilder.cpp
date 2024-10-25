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
    if ((blockType == GRASS && vert->normal.y > 0.0f) || (blockType == TALLGRASS)) {
        return color;
    } else if (blockType == LEAVES) {
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

bool isHidden(World* world, int x, int y, int z, Block* currentBlock, glm::vec3 normal) {
    if (!currentBlock) {
        return true;
    }
    // Calculate adjacent block coordinates
    int adjX = x + static_cast<int>(normal.x);
    int adjY = y + static_cast<int>(normal.y);
    int adjZ = z + static_cast<int>(normal.z);

    // Get the adjacent block
    Block* adjacentBlock = world->getBlock(adjX, adjY, adjZ);
    // No adjacent Block, not hidden
    if (!adjacentBlock) {
        return true;
    }

    // If it's the same as the checking block
    if (currentBlock->getBlockType() >= FLOWING_WATER && currentBlock->getBlockType() <= LAVA || 
        !currentBlock->getPartialBlock() && currentBlock->getTransparent() && currentBlock->getBlockType() != LEAVES) {
        if (!adjacentBlock->getTransparent() && normal.y <= 0.0) {
            return true;
        }
        if (adjacentBlock->getBlockType() == currentBlock->getBlockType() && currentBlock->getBlockMetaData() == 0) {
            return true;
        }
    } else {
        if (adjacentBlock->getTransparent() || adjacentBlock->getPartialBlock() || currentBlock->getPartialBlock()) {
            return false;
        } else {
            //return adjacentBlock->getBlockType();
            return true;
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
    if (blockType == AIR) {
        return nullptr;
    }

    // Snow
    if (blockType == GRASS) {
        Block* b = world->getBlock(x,y+1,z);
        if (b && b->getBlockType() == SNOW_LAYER) {
            specialQuery = "Snow";
        }
    }

    // Bed
    if (blockType == BED) {
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
            if (blockType == LEAVES ||
                blockType == LOG ||
                blockType == TALLGRASS ||
                blockType == TORCH ||
                blockType == OAK_STAIRS ||
                blockType == STONE_STAIRS ||
                blockType == WOOL ||
                blockType == PISTON ||
                blockType == TRAPDOOR) {
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

float getAmbientOcclusion(World* world, glm::vec3 position, glm::vec3 vertexPosition, glm::vec3 normal) {
    Block* b1;
    Block* b2;
    Block* bc;
    glm::vec3 off1;
    glm::vec3 off2;
    glm::vec3 offc;
    if (normal.x != 0.0f) {
        off1 = position + glm::vec3(vertexPosition.x, vertexPosition.y,-vertexPosition.z);
        off2 = position + glm::vec3(vertexPosition.x,-vertexPosition.y, vertexPosition.z);
        offc = position + glm::vec3(vertexPosition.x, vertexPosition.y, vertexPosition.z);
    }
    if (normal.y != 0.0f) {
        off1 = position + glm::vec3( vertexPosition.x,vertexPosition.y,-vertexPosition.z);
        off2 = position + glm::vec3(-vertexPosition.x,vertexPosition.y, vertexPosition.z);
        offc = position + glm::vec3( vertexPosition.x,vertexPosition.y, vertexPosition.z);
    }
    if (normal.z != 0.0f) {
        off1 = position + glm::vec3(-vertexPosition.x, vertexPosition.y,vertexPosition.z);
        off2 = position + glm::vec3( vertexPosition.x,-vertexPosition.y,vertexPosition.z);
        offc = position + glm::vec3( vertexPosition.x, vertexPosition.y,vertexPosition.z);
    }
    b1 = world->getBlock(floor(off1.x), floor(off1.y), floor(off1.z));
    b2 = world->getBlock(floor(off2.x), floor(off2.y), floor(off2.z));
    bc = world->getBlock(floor(offc.x), floor(offc.y), floor(offc.z));
    int side1  = 0;
    int side2  = 0;
    int corner = 0;
    if (b1 && !b1->getTransparent() && !b1->getPartialBlock()) { side1  = b1->getBlockType(); }
    if (b2 && !b2->getTransparent() && !b2->getPartialBlock()) { side2  = b2->getBlockType(); }
    if (bc && !bc->getTransparent() && !bc->getPartialBlock()) { corner = bc->getBlockType(); }

    // Convert block existence to integer (1 if block exists, 0 otherwise)
    int side1Int = side1 ? 1 : 0;
    int side2Int = side2 ? 1 : 0;
    int cornerInt = corner ? 1 : 0;

    // Apply vertexAO formula
    float ao = 4.0f - static_cast<float>(side1Int + side2Int + cornerInt);

    // Normalize the AO level (between 0 and 1)
    ao /= 4.0f;

    return ao;
}

float getSmoothLighting(World* world, glm::vec3 position, glm::vec3 vertexPosition, glm::vec3 normal, uint8_t maxSkyLight) {
    float x = position.x;
    float y = position.y;
    float z = position.z;
    int light = 0;
    int relevantLights = 0;
    Block* b = nullptr;

    // Get the adjacent blocks along face
    for (int aOff = -1; aOff < 1; aOff++) {
        for (int bOff = -1; bOff < 1; bOff++) {
            if (normal.x != 0.0) {
                b = world->getBlock(floor(x+normal.x*0.9), floor(y+aOff), floor(z+bOff));
            } else if (normal.y != 0.0) {
                b = world->getBlock(floor(x+aOff), floor(y+normal.y*0.9), floor(z+bOff));
            } else if (normal.z != 0.0) {
                b = world->getBlock(floor(x+aOff), floor(y+bOff), floor(z+normal.z*0.9));
            } else {
                b = nullptr;
            }
            if (b) {
                // Air is transparent, so we can ignore it too
                if (b->getTransparent() && !b->getPartialBlock()) {
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

    // Ensure light index is within bounds
    if (light < 0) {
        light = 0;
    } else if (light > 15) {
        light = 15;
    }

    return lightArray[light];
}

glm::vec3 rotateVertexAroundOrigin(glm::vec3 vertexPosition, float angle, glm::vec3 axis) {
    // Create a rotation matrix for the given angle and axis
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    
    // Apply the rotation to the vertex position
    glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(vertexPosition, 1.0f);
    
    // Return the rotated vertex position as a vec3
    return glm::vec3(rotatedPosition);
}


std::vector<DummyMesh> ChunkBuilder::buildChunks(std::vector<Chunk*> chunks, bool smoothLighting, uint8_t maxSkyLight) {
    std::vector<DummyMesh> meshes;
    for (auto c : chunks) {
        meshes.push_back(buildChunk(c,smoothLighting,maxSkyLight));
    }
    return meshes;
}

void rotateBlockAccordingToMetaData(glm::vec3& vertPos, uint8_t& blockType, uint8_t& blockMetaData) {
    if (blockType == STANDING_SIGN) {
        vertPos = rotateVertexAroundOrigin(vertPos, (16-float(blockMetaData))*22.5f, glm::vec3(0.0,1.0,0.0));
    }
    if (blockType == LADDER) {
        switch(blockMetaData) {
            case 3: // South
                vertPos = rotateVertexAroundOrigin(vertPos, 180, glm::vec3(0.0,1.0,0.0));
                break;
            case 4: // West
                vertPos = rotateVertexAroundOrigin(vertPos, 90, glm::vec3(0.0,1.0,0.0));
                break;
            case 5: // East
                vertPos = rotateVertexAroundOrigin(vertPos, -90, glm::vec3(0.0,1.0,0.0));
                break;
            default: // North
                break;
        }
    }
}

DummyMesh ChunkBuilder::buildChunk(Chunk* chunk, bool smoothLighting, uint8_t maxSkyLight) {
    std::vector<Vertex> worldVertices;
    std::vector<GLuint> worldIndices;

    std::vector<Vertex> waterVertices;
    std::vector<GLuint> waterIndices;

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
                Mesh* mesh = cachedMesh;
                if (!mesh) {
                    continue;
                }
                glm::vec3 offset;
                for (uint v = 0; v < mesh->vertices.size(); v++) {
                    if (isHidden(world,x,y,z,b,mesh->vertices[v].normal)) {
                        continue;
                    }
                    glm::vec3 color = getBiomeBlockColor(blockType, blockMetaData, &mesh->vertices[v]);

                    // Liquid change model
                    // If we're dealing with the upper verts of  water or lava
                    if (blockType >= 8 && blockType <= 11 && mesh->vertices[v].position.y > 0.2f) {
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

                    glm::vec3 vertPos = glm::vec3(mesh->vertices[v].position + offset);
                    rotateBlockAccordingToMetaData(vertPos,blockType,blockMetaData);
                    glm::vec3 worldPos = vertPos + pos + 0.5f;
                    glm::vec2 finalUV = mesh->vertices[v].textureUV;
                    // Only affects the side
                    if (mesh->vertices[v].normal.y == 0) {
                        finalUV.y = finalUV.y + (offset.y/16);
                    }
                    
                    // Water in it's own thing
                    if (blockType == WATER || blockType == LAVA) {
                        color *= getLighting(world,x,y,z,mesh->vertices[v].normal, maxSkyLight);
                        waterVertices.push_back(
                            Vertex(
                                worldPos,
                                mesh->vertices[v].normal,
                                color,
                                finalUV
                            )
                        );
                    } else {
                        // Apply light if the block isn't a lightsource
                        if (!b->lightSource) {
                            if (smoothLighting) {
                                color *= getSmoothLighting(world,worldPos,vertPos, mesh->vertices[v].normal,maxSkyLight);
                                if (!b->getTransparent() ) {
                                    color *= getAmbientOcclusion(world,worldPos,vertPos, mesh->vertices[v].normal);
                                }
                            } else {
                                color *= getLighting(world,x,y,z,mesh->vertices[v].normal, maxSkyLight);
                            }
                        }
                        worldVertices.push_back(
                            Vertex(
                                worldPos,
                                mesh->vertices[v].normal,
                                color,
                                finalUV
                            )
                        );
                    }
                }

                GLuint totalWaterIndices = waterIndices.size();
                GLuint totalWorldIndices = worldIndices.size();

                if (blockType == WATER || blockType == LAVA) {
                    for (uint i = 0; i < mesh->indices.size(); i++) {
                        GLuint newInd = totalWaterIndices + mesh->indices[i];
                        waterIndices.push_back(newInd);
                    }
                } else {
                    for (uint i = 0; i < mesh->indices.size(); i++) {
                        GLuint newInd = totalWorldIndices + mesh->indices[i];
                        worldIndices.push_back(newInd);
                    }
                }
            }
        }
    }

    return DummyMesh(chunk, std::to_string(chunkX) + "_" + std::to_string(chunkZ),
        worldVertices, worldIndices, waterVertices, waterIndices);
}